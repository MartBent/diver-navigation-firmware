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
#include "UI/image.c"
#include <gps.h>

#define configUSE_TIME_SLICING 1
#define LV_TICK_PERIOD_MS 1

#define BUTTON1 27
#define BUTTON2 19
#define BUTTON3 22
#define BUTTON4 21

static TaskHandle_t button1_task_handle = NULL;
static TaskHandle_t button2_task_handle = NULL;
static TaskHandle_t button3_task_handle = NULL;
static TaskHandle_t button4_task_handle = NULL;

//Task for receiving any LoRa messages and processing them.
static void loraTask(void *pvParameter)
{
  while(1) {
    uint8_t data[20480] = {};
    uint8_t length = lora_receive(data);    
    if(length > 1) {
      switch(data[0]) {
        case 0: {
          CommunicationMessage* msg = malloc(sizeof(CommunicationMessage));
          decodeCommMessage(data, length, msg);
          processCommunicationMessage(msg);
          free(msg);
          break;
        }
        case 1: {
          GpsMessage* msg = malloc(sizeof(GpsMessage));
          decodeGpsMessage(data, msg);
          processGpsMessage(msg);
          free(msg);
          break;
        }
         case 2: {
          printf("rx map: %d", length);
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

    if(isDiving) {
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
    lv_label_set_text_fmt(menu_screen->lbl_battery, "Battery: %d %%", 100);
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