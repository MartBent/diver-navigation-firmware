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
#include "UI/image.c"
#include "UI/font.c"
#include "Lora/Lora.h"

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

static lv_obj_t* lbl_screen;
static lv_obj_t* lbl;

static lv_obj_t* map_screen;
static lv_obj_t* map;

static uint8_t bufferedData[128] = {};
static uint8_t bufferedDataLenth = 0;

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
  
  //lcdDrawMultiPixels(&dev, area->x1, area->x2, size, &color_p->full);
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
    uint8_t len = lora_receive(data);
    printf((char*)data);
    vTaskDelay(8000 / portTICK_PERIOD_MS);
  }
}

lv_obj_t* getButton(char* text, lv_color_t color) {

  lv_obj_t* bkgrnd = lv_obj_create(map_screen, NULL);
  lv_obj_set_width(bkgrnd, 40);
	lv_obj_set_height(bkgrnd, 20);
	lv_obj_t* label = lv_label_create(bkgrnd, NULL);
	lv_label_set_text(label, text);
  
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK );
  lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_8);
	lv_obj_set_style_local_bg_color(bkgrnd, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, color);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
  return bkgrnd;
}

static int counter = 0;

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

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 128;
  disp_drv.ver_res = 160;
  disp_drv.sw_rotate = true;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  
  lv_disp_drv_register(&disp_drv);
  lv_disp_set_rotation(NULL, 315);

  map_screen = lv_obj_create(NULL, NULL);
  lbl_screen = lv_obj_create(NULL, NULL);

  lbl = lv_label_create(lbl_screen, NULL);
  lv_label_set_text_fmt(lbl, "Test %d", counter);

  map = lv_img_create(map_screen, NULL);
  lv_img_set_src(map, &hilgelo);
  lv_obj_align(map, NULL, LV_ALIGN_CENTER, 0, 30);

  lv_obj_t* button = getButton("OK", LV_COLOR_GREEN);
  lv_obj_align(button, NULL, LV_ALIGN_IN_TOP_LEFT, 3, 3);

  lv_obj_t* button2 = getButton("Call", LV_COLOR_RED);
  lv_obj_align(button2, NULL, LV_ALIGN_IN_TOP_LEFT, 43, 3);

  lv_obj_t* button3 = getButton("Settings", LV_COLOR_BLUE);
  lv_obj_align(button3, NULL, LV_ALIGN_IN_TOP_LEFT, 83, 3);

  lv_obj_t* button4 = getButton("Messaging", LV_COLOR_YELLOW);
  lv_obj_align(button4, NULL, LV_ALIGN_IN_TOP_LEFT, 123, 3);

  lv_scr_load(lbl_screen);

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

    if(lv_scr_act() == map_screen) {
      lv_scr_load(lbl_screen);
    } else {
      lv_scr_load(map_screen);
    }
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
    lv_label_set_text_fmt(lbl, "Test %d", counter);
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
    lv_label_set_text_fmt(lbl, "Test %d", ++counter);
    lv_task_handler();
    lv_tick_inc(10);
    vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
} 