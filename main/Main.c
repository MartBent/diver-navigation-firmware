#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "Lora/Lora.c"

#include "UI/display.c"
#include "UI/screens/screens.c"

#include <gps.h>
#include <zlib.h>
#include "stats/depth.c"


#define configUSE_TIME_SLICING 1
#define LV_TICK_PERIOD_MS 1

#define BUTTON1 15
#define BUTTON2 22
#define BUTTON3 19
#define BUTTON4 21

static TaskHandle_t button1_task_handle = NULL;
static TaskHandle_t button2_task_handle = NULL;
static TaskHandle_t button3_task_handle = NULL;
static TaskHandle_t button4_task_handle = NULL;


//Task for receiving any LoRa messages and processing them.
static void loraTask(void *pvParameter)
{
  while(1) {
    uint8_t data[128] = {};
    uint8_t length = lora_receive(data);
    if(length > 1) {
      printf("RX lora %d \n", length);
      switch(data[0]) {
        case 0: {
          if(!isSyncing) {
            CommunicationMessage* msg = malloc(sizeof(CommunicationMessage));
            decodeCommMessage(data, length, msg);
            processCommunicationMessage(msg);
            free(msg);
          }
          break;
        }
        case 1: {
          if(!isSyncing) {
            GpsMessage* msg = malloc(sizeof(GpsMessage));
            decodeGpsMessage(data, msg);
            processGpsMessage(msg, true);
            free(msg);
          }
          break;
        }
        case 72: {
          if(!isSyncing) {
            GpsMessage* msg = malloc(sizeof(GpsMessage));
            decodeGpsMessage(data, msg);
            processGpsMessage(msg, false);
            free(msg);
          }
          break;
        }
        case 2: {
          //The first sync message will give the length and coordinates.
          if(length == 19 && compressed_map_index == 0) {
            
            printf("\n");
            memcpy(&compressed_length, &data[1], 2);
            compressed_map = malloc(compressed_length);

            frame_amount = (uint16_t)ceil((double)compressed_length / 125);

            memcpy(&rx_latitude, &data[3], 8);
            memcpy(&rx_longtitude, &data[11], 8);

            printf("Started map sync at %0.5f, %0.5f, Length: %ld, Frames: %d\n", rx_latitude, rx_longtitude, compressed_length, frame_amount);

            uint8_t ack[1] = {0x03};
            lora_send_bytes(ack, 1);

            isSyncing = true;
            lv_label_set_text(menu_screen->menu_options[2]->label, "Syncing");
          }
          else {
            uint16_t rx_index = 0;

            memcpy(&rx_index, &data[1], 2);
            
            printf("RX map, index %d\n", rx_index);

            if(rx_index == frame_amount-1) { //Final fragment

              //Data[Index, Index[0], Index[1], Map[0] .... Map[n]]
              memcpy(&compressed_map[rx_index*125], &data[3], length-3); //Length - 3 to skip index + ID
              
              uint8_t ack[3] = {0x03, 0,0};
              memcpy(&ack[1], &compressed_map_index, 2);
              printf("Sending ACK %d\n", compressed_map_index);
              lora_send_bytes(ack, 3);

              uint8_t* decompressed = malloc(40960);
              uLongf decompressedLen = 40960;
              uncompress(decompressed, &decompressedLen, compressed_map, compressed_length);
              
              if(decompressedLen == 40960) {
                //Save the map to flash memory
                saveMap(decompressed, rx_latitude, rx_longtitude);
                printf("Map synced at %0.5f, %0.5f, Len %ld\n", rx_latitude, rx_longtitude, decompressedLen);
              } else {
                printf("Map sync failed, try again");
              }
              free(decompressed);

              //Refresh map from flash
              retrieveMap(map, &center_latitude, &center_longtitude);
              lv_img_set_src(map_screen->map, map_src);

              center_latitude = rx_latitude;
              center_longtitude = rx_longtitude;

              free(compressed_map);

              //Reset variables after syncing
              isSyncing = false;
              compressed_length = 0;
              compressed_map_index = 0;
              frame_amount = 0;
              rx_latitude = 0;
              rx_longtitude = 0;

              //Set back the button text
              lv_label_set_text(menu_screen->menu_options[2]->label, "Not Sync");
            }

            //If index is correct, append to compressed map
            else if(rx_index == compressed_map_index && length == 128) {
              printf("Appending map at %d\n", compressed_map_index*125);
              memcpy(&compressed_map[compressed_map_index*125], &data[3], length-3); //Length - 3 to skip index + ID

              rx_frame_amount++;
              currentSyncPercentage = ceil((float)100 * (float)rx_frame_amount) / (float)frame_amount;
             
              uint8_t ack[3] = {0x03, 0,0};
              memcpy(&ack[1], &compressed_map_index, 2);
              printf("Sending ACK %d\n", compressed_map_index);
              lora_send_bytes(ack, 3);
              compressed_map_index++;
            }

            else if(rx_index == compressed_map_index-1 && length == 128) { //Resend
              uint16_t old_index = compressed_map_index-1;
              printf("RX map, index %d, ACK RESEND\n", old_index); //Index - 1 since its a resend
              uint8_t ack[3] = {0x03, 0,0};
              memcpy(&ack[1], &old_index, 2);
              lora_send_bytes(ack, 3);
            }
          }
          break;
        }
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

static void gpsTask(void* pvParameter) {
  GPSModuleCoordinates* coords = malloc(sizeof(GPSModuleCoordinates));
  GpsMessage* msg = malloc(sizeof(GpsMessage));
  uint8_t data[17] = {};
  while(1) {

    read_gps_coordinates(coords);
    adjustLocationMarker(coords->latitude, coords->longtitude);

    if(isDiving && !isSyncing) {
      msg->latitude = coords->latitude;
      msg->longitude = coords->longtitude;
      encodeGpsMessage(msg, data);
      lora_send_bytes(data, 17);
    }

    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

//For showing the time on the menu screen
static void statsTask(void* pvParameter) {
  static uint8_t seconds = 0;
  static uint64_t minutes = 0;
  
  while(1) {
    if(isDiving) {
      ++seconds;
      if(seconds >= 60) {
        seconds = 0;
        ++minutes;
      }
      lv_label_set_text_fmt(menu_screen->lbl_time, "Time: %lld:%d", minutes, seconds);
    } else {
      seconds = 0;
      minutes = 0;
      lv_label_set_text_fmt(menu_screen->lbl_time, "Time: 0:0");
    }
    lv_label_set_text_fmt(menu_screen->lbl_depth, "Depth: %dm", getCurrentDepth());
    if(isSyncing) {
    lv_label_set_text_fmt(menu_screen->lbl_sync, "Syncing: %d%%", currentSyncPercentage);
    } else {
      lv_label_set_text(menu_screen->lbl_sync, "");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//Task for refreshing the display with new data.
static void guiTask(void *pvParameter)
{
  (void) pvParameter;

  while (1) {

    //Disable button interrupts when refreshing the screen
    gpio_intr_disable(BUTTON1);
    gpio_intr_disable(BUTTON2);
    gpio_intr_disable(BUTTON3);
    gpio_intr_disable(BUTTON4);

    lv_task_handler();
    lv_tick_inc(10);

    //Reanable
    gpio_intr_enable(BUTTON1);
    gpio_intr_enable(BUTTON2);
    gpio_intr_enable(BUTTON3);
    gpio_intr_enable(BUTTON4);
    vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
} 

void IRAM_ATTR button_isr(void* arg) {
  vTaskResume(*(TaskHandle_t*)arg);
}

void button1_handler(void* arg)
{
  while(1) {

    vTaskSuspend(button1_task_handle);
    printf("Btn 1\n");
    switch(getCurrentScreen()) {
      case MAP_SCREEN: {
        handleMapScreenButton(1);
        break;
      }
      case CONFIG_SCREEN: {
        handleConfigScreenButton(1);
        break;
      }
      case MENU_SCREEN: {
        handleMenuScreenButton(1);
        break;
      }
      case MESSAGE_SCREEN: {
        handleMessageScreenButton(1);
        break;
      }
    }

    //Delay for debounce
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void button2_handler(void* arg)
{
  while(1) {
    vTaskSuspend(button2_task_handle);
    printf("Btn 2\n");
    switch(getCurrentScreen()) {
      case MAP_SCREEN: {
        handleMapScreenButton(2);
        break;
      }
      case CONFIG_SCREEN: {
        handleConfigScreenButton(2);
        break;
      }
      case MENU_SCREEN: {
        handleMenuScreenButton(2);
        break;
      }
      case MESSAGE_SCREEN: {
        handleMessageScreenButton(2);
        break;
      }
    }

    //Delay for debounce
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void button3_handler(void* arg)
{
  while(1) {
    vTaskSuspend(button3_task_handle);
    printf("Btn 3\n");
    switch(getCurrentScreen()) {
      case MAP_SCREEN: {
        handleMapScreenButton(3);
        break;
      }
      case CONFIG_SCREEN: {
        handleConfigScreenButton(3);
        break;
      }
      case MENU_SCREEN: {
        handleMenuScreenButton(3);
        break;
      }
      case MESSAGE_SCREEN: {
        handleMessageScreenButton(3);
        break;
      }
    }

    //Delay for debounce
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void button4_handler(void* arg)
{
  while(1) {
    vTaskSuspend(button4_task_handle);
    printf("Btn 4\n");
    switch(getCurrentScreen()) {
      case MAP_SCREEN: {
        handleMapScreenButton(4);
        break;
      }
      case CONFIG_SCREEN: {
        handleConfigScreenButton(4);
        break;
      }
      case MENU_SCREEN: {
        handleMenuScreenButton(4);
        break;
      }
      case MESSAGE_SCREEN: {
        handleMessageScreenButton(4);
        break;
      }
    }

    //Delay for debounce
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  setup_lv();
  initScreens();

  //Setup communication
  setup_lora();

  //Setup GPS
  setup_gps(NULL);

  //Initialize the buttons
  gpio_pad_select_gpio(BUTTON1);
	gpio_set_direction(BUTTON1, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON1, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(BUTTON1, GPIO_INTR_NEGEDGE);

  gpio_pad_select_gpio(BUTTON2);
	gpio_set_direction(BUTTON2, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON2, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(BUTTON2, GPIO_INTR_NEGEDGE);

  gpio_pad_select_gpio(BUTTON3);
	gpio_set_direction(BUTTON3, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON3, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(BUTTON3, GPIO_INTR_NEGEDGE);

  gpio_pad_select_gpio(BUTTON4);
	gpio_set_direction(BUTTON4, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON4, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(BUTTON4, GPIO_INTR_NEGEDGE);

  gpio_intr_enable(BUTTON1);
  gpio_intr_enable(BUTTON2);
  gpio_intr_enable(BUTTON3);
  gpio_intr_enable(BUTTON4);

  gpio_install_isr_service(0);

  gpio_isr_handler_add(BUTTON1, button_isr, &button1_task_handle);
  gpio_isr_handler_add(BUTTON2, button_isr, &button2_task_handle);
  gpio_isr_handler_add(BUTTON3, button_isr, &button3_task_handle);
  gpio_isr_handler_add(BUTTON4, button_isr, &button4_task_handle);

  xTaskCreate(button1_handler, "button 1 isr", 2048, NULL, 5, &button1_task_handle);
  xTaskCreate(button2_handler, "button 2 isr", 2048, NULL, 5, &button2_task_handle);
  xTaskCreate(button3_handler, "button 3 isr", 2048, NULL, 5, &button3_task_handle);
  xTaskCreate(button4_handler, "button 4 isr", 2048, NULL, 5, &button4_task_handle);

  //Start the periodic tasks
  xTaskCreate(guiTask, "gui", 2048, NULL, 1, NULL);
  xTaskCreate(loraTask, "lora", 40960, NULL, 5, NULL);
  xTaskCreate(statsTask, "stats", 2048, NULL, 5, NULL);
  xTaskCreate(gpsTask, "gps", 2048, NULL, 5, NULL);
}