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
#include "lvgl.h"
#include "st7789.h"

#define configUSE_TIME_SLICING 0
#define LV_TICK_PERIOD_MS 1

static void guiTask(void *pvParameter);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static TFT_t dev;
static uint32_t counter = 0;

static lv_obj_t* label;

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;
  uint32_t size = 240 * 135;

   for (int y = area->y1; y <= area->y2; y++) {
      for (int x = area->x1; x <= area->x2; x++) {

          c = color_p->full;
          lcdDrawPixel(&dev, x, y, c);
          color_p++;
      }
    }
  //lcdDrawMultiPixels(&dev, area->x1, area->x2, size, &color_p->full);
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}


static void incTask(void *pvParameter)
{
  while(1) {
    counter++;
    lv_label_set_text_fmt(label, "Hello esp %d!", counter);
    printf("Inc");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void app_main()
{
  lv_init();

  //Mosi, Sclk, cs, rst, backlight, touchscreen stuff,
  spi_master_init(&dev, 19, 18, 5, 16, 23, 4);
  
  int width = 135;
  int height = 240;
  int xoffset = 52;
  int yoffset = 40;

  lcdInit(&dev, width, height, xoffset, yoffset);

  lcdFillScreen(&dev, 0);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 135;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /* Create simple label */
  label = lv_label_create(lv_scr_act(), NULL);
  lv_obj_t *label2 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Hello esp 1!");
  lv_label_set_text(label2, "Hello esp 2!");
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_obj_align(label2, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  lv_task_handler();
  lv_tick_inc(20);

  xTaskCreate(incTask, "inc", 2048, NULL, 5, NULL);
  xTaskCreate(guiTask, "gui", 4096, NULL, 5, NULL);
}

static void guiTask(void *pvParameter)
{
  (void) pvParameter;
  while (1) {
    lv_task_handler();
    lv_tick_inc(10);
    printf("Gui");
    vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
}