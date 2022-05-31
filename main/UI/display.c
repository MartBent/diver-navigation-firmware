#ifndef DISPLAY_C
#define DISPLAY_C

#include "lvgl.h"
#include "st7789.c"
#include "screens.c"

//SPI Pins
#define SCREEN_MOSI 23
#define SCREEN_SCK 18

//GPIO
#define SCREEN_CS 25
#define SCREEN_DC 26
#define SCREEN_RST 0
#define SCREEN_BL 14

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
static TFT_t dev;

static MessageScreen* message_screen;
static MapScreen* map_screen;
static ConfigScreen* config_screen;
static MenuScreen* menu_screen;

typedef enum {
    MAP_SCREEN,
    MESSAGE_SCREEN,
    STATS_SCREEN,
    CONFIG_SCREEN,
    MENU_SCREEN
} screen_t;

//A function that is passed to the LVGL driver to edit the display.
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  uint32_t wh = w*h;

  lcdSetAddrWindow(&dev, area->x1, area->y1, area->x2, area->y2);
  while (wh--) lcdPushColor(&dev, color_p++->full);
  lv_disp_flush_ready(disp);
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
  menu_screen = createMenuScreen();

  lv_scr_load(menu_screen->root);

  lv_task_handler();
  lv_tick_inc(20);
}

screen_t getCurrentScreen() {
    if(lv_scr_act() == map_screen->root) {
        return MAP_SCREEN;
    }
    if(lv_scr_act() == config_screen->root) {
        return CONFIG_SCREEN;
    }
    if(lv_scr_act() == message_screen->root) {
        return MESSAGE_SCREEN;
    }
    if(lv_scr_act() == menu_screen->root) {
        return MENU_SCREEN;
    }
    if(lv_scr_act() == map_screen->root) {
        return STATS_SCREEN;
    }
    return MAP_SCREEN;
}

void handleMessageScreenButton(uint8_t button_num) {
  lv_scr_load(config_screen->root);
}
void handleConfigScreenButton(uint8_t button_num) {
  lv_scr_load(map_screen->root);
}
void handleMenuScreenButton(uint8_t button_num) {

  for(int i = 0; i < 4; i++) 
  {
    lv_obj_set_style_local_bg_color(menu_screen->menuOptions[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  }
  if(menu_screen->current_option >= 3) 
  {
    menu_screen->current_option = 0;
  } 
  else 
  {
    menu_screen->current_option++;
  }

  lv_obj_set_style_local_bg_color(menu_screen->menuOptions[menu_screen->current_option], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN); 
}

void handleMapScreenButton(uint8_t button_num) {
  lv_scr_load(message_screen->root);
}
#endif