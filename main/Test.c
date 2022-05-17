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
#include "st7789.h"
#include "image.c"

//#define configUSE_TIME_SLICING 0
#define LV_TICK_PERIOD_MS 1


static void guiTask(void *pvParameter);
static void loraTask(void *pvParameter);

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static TFT_t dev;
static uint32_t counter = 0;

static lv_obj_t* map;

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
  lv_disp_flush_ready(disp);
}

static void incTask(void *pvParameter)
{
  while(1) {
    counter++;
    printf("Inc\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

static void loraTask(void *pvParameter)
{
  while(1) {
    printf("Sending lora...\n");
    uart_write_bytes(UART_NUM_2, (const char*)"Hello world", strlen("Hello world"));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup_lv() {
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
  map = lv_img_create(lv_scr_act(), NULL);

  lv_img_set_src(map, &staticmap);
  lv_obj_align(map, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_img_set_angle(map, 900);
  lv_task_handler();
  lv_tick_inc(20);

  xTaskCreate(guiTask, "gui", 4096, NULL, 5, NULL);
}

void setup_lora() {
    const uart_port_t uart_num = UART_NUM_2;
  uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122, 
  };
  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

  ESP_ERROR_CHECK(uart_set_pin(uart_num, 17, 16, 0, 0));

  // Setup UART buffered IO with event queue
  const int uart_buffer_size = (1024 * 2);
  QueueHandle_t uart_queue;
  
  ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

  // Write data to UART. 
  char* test_str = "Hello world\n";
  while(1) {;
    uint8_t data[128] = {};
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    length = uart_read_bytes(uart_num, data, length, 100);
    printf((char*)data);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  
}
void app_main()
{
  //setup_lv();
  setup_lora();

  //xTaskCreate(incTask, "inc", 2048, NULL, 5, NULL);
  //xTaskCreate(guiTask, "gui", 2048, NULL, 5, NULL);
  //xTaskCreate(loraTask, "lora", 2048, NULL, 5, NULL);
}

static void guiTask(void *pvParameter)
{
  (void) pvParameter;
  while (1) {
    lv_task_handler();
    lv_tick_inc(10);
    vTaskDelay( 10 / portTICK_PERIOD_MS);
  }
} 