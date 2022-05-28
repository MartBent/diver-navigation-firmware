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
#include "driver/gpio.h"
#include "driver/uart.h"
#include "lvgl.h"
#include "UI/st7789.h"
#include "Lora/Lora.h"
#include "UI/screens.h"

#define configUSE_TIME_SLICING 1
#define LV_TICK_PERIOD_MS 1

//SPI Pins
#define SCREEN_MOSI 23
#define SCREEN_SCK 18

//GPIO
#define SCREEN_CS 25
#define SCREEN_DC 26
#define SCREEN_RST 0
#define SCREEN_BL 14

#define BUTTON 27

static void guiTask(void *pvParameter);
static void loraTask(void *pvParameter);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static TFT_t dev;

static MessageScreen* message_screen;
static MapScreen* map_screen;
static ConfigScreen* config_screen;

static uint8_t bufferedData[128] = {};
static uint8_t bufferedDataLenth = 0;

static int counter = 0;

//A function that is passed to the LVGL driver to edit the display.
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {

        c = color_p->full;
        lcdDrawPixel(&dev, x, y, c);
        color_p++;
    }
  }
  lv_disp_flush_ready(disp);
}

//Task for receiving any LoRa messages and processing them.
static void loraTask(void *pvParameter)
{
  while(1) {
    printf("Lora");
    if(bufferedDataLenth > 0) {
      lora_send(bufferedData, bufferedDataLenth);
      bufferedDataLenth = 0;
    }

    uint8_t data[128] = {};
    lora_receive(data);
    printf((char*)data);
    vTaskDelay(8000 / portTICK_PERIOD_MS);
  }
}

//Sets up the screen, implemented for the ESP32 TDisplay board.
void setup_lv() {
  lv_init();

  //Mosi, Sclk, cs, rst, backlight, touchscreen stuff,
  spi_master_init(&dev, SCREEN_MOSI, SCREEN_SCK, SCREEN_CS, SCREEN_DC, SCREEN_RST, SCREEN_BL);
  int width = 128;
  int height = 160;
  int xoffset = 0;
  int yoffset = 0;

  lcdInit(&dev, width, height, xoffset, yoffset);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  //Display creation
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 128;
  disp_drv.ver_res = 160;
  disp_drv.sw_rotate = true;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  
  lv_disp_drv_register(&disp_drv);
  lv_disp_set_rotation(NULL, 315);

  map_screen = createMapScreen();
  message_screen = createMessageScreen();
  config_screen = createConfigScreen();

  lv_scr_load(message_screen->root);

  lv_task_handler();
  lv_tick_inc(20);
}

TaskHandle_t ISR = NULL;

void IRAM_ATTR button_isr(void* arg) {
  vTaskResume(ISR);
}

void gpioHandler(void* arg)
{
  while(1) {
    vTaskSuspend(ISR);
    printf("Press");

    if(lv_scr_act() == map_screen->root) {
      lv_scr_load(message_screen->root);
    } else if(lv_scr_act() == message_screen->root) {
      lv_scr_load(config_screen->root);
    }
    else if(lv_scr_act() == config_screen->root) {
      lv_scr_load(map_screen->root);
    }

    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  gpio_pad_select_gpio(BUTTON);
	gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);
	gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE);

  setup_lv();
  setup_lora();

  gpio_intr_enable(BUTTON);
  gpio_install_isr_service(0);

  gpio_isr_handler_add(BUTTON, button_isr, NULL);

  xTaskCreate(gpioHandler, "isr", 2048, NULL, 5, &ISR);

  while (1) {
    lv_task_handler();
    lv_tick_inc(10);
    vTaskDelay(10 / portTICK_PERIOD_MS);

    if(bufferedDataLenth > 0) {
      lora_send(bufferedData, bufferedDataLenth);
      bufferedDataLenth = 0;
    }

    uint8_t data[128] = {};
    uint8_t len = lora_receive(data);
    if(len > 0) {
      printf((char*)data);
    }
  }

  //xTaskCreate(guiTask, "gui", 2048, NULL, 5, NULL);
  //xTaskCreate(loraTask, "lora", 2048, NULL, 5, NULL);
}

//Task for refreshing the display with new data.
static void guiTask(void *pvParameter)
{
  (void) pvParameter;
  while (1) {
    lv_task_handler();
    lv_tick_inc(10);
    vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
} 