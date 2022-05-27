#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "lvgl.h"
#include "UI/st7789.h"
#include "UI/image.c"
#include "UI/font.c"
#include "Lora/Lora.h"

//#define configUSE_TIME_SLICING 0
#define LV_TICK_PERIOD_MS 1

//SPI Pins
#define SCREEN_MOSI 23
#define SCREEN_SCK 18

//GPIO
#define SCREEN_CS 25
#define SCREEN_DC 26
#define SCREEN_RST 0
#define SCREEN_BL 14

#define LED 5

static void guiTask(void *pvParameter);
static void loraTask(void *pvParameter);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static TFT_t dev;

static lv_obj_t* lbl_screen;
static lv_obj_t* lbl;

static lv_obj_t* map_screen;
static lv_obj_t* map;

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

  lv_scr_load(map_screen);

  lv_task_handler();
  lv_tick_inc(20);
}

void app_main()
{
  gpio_reset_pin( LED );
  gpio_set_direction( LED, GPIO_MODE_OUTPUT );
  gpio_set_level( LED, 0 );

  setup_lv();
  setup_lora();

  xTaskCreate(guiTask, "gui", 2048, NULL, 5, NULL);
  xTaskCreate(loraTask, "lora", 2048, NULL, 5, NULL);
}

//Task for refreshing the display with new data.
static void guiTask(void *pvParameter)
{
  (void) pvParameter;
  while (1) {
    printf("UI\n");
    lv_task_handler();
    lv_tick_inc(10);
    vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
} 