#ifndef SCREENS_C
#define SCREENS_C

#include "lvgl.h"


typedef struct {
    lv_obj_t* back;
    lv_obj_t* label;
} button_t;

typedef struct {
    lv_obj_t* root;
    lv_obj_t* message_box;
    lv_obj_t* message_options[5];
    lv_obj_t* btn_up;
    lv_obj_t* btn_down;
    lv_obj_t* btn_clear;
    uint8_t current_option;
} MessageScreen;

typedef struct  {
    lv_obj_t* root;
    lv_obj_t* label;
    lv_obj_t* btn_up;
    lv_obj_t* btn_down;
} ConfigScreen;

typedef struct  {
    lv_obj_t* root;
    button_t* menu_options[4];
    lv_obj_t* btn_up;
    lv_obj_t* btn_down;
    uint8_t current_option;
    lv_obj_t* lbl_depth;
    lv_obj_t* lbl_sync;
    lv_obj_t* lbl_time;
} MenuScreen;

typedef struct {
    lv_obj_t* root;
    lv_obj_t* map;
    lv_obj_t* location_marker;
    lv_obj_t* location_boat_marker;
} MapScreen;

uint8_t scrollMenu(uint8_t index, uint8_t size, bool isUp) {
    uint8_t max = size - 1;
    uint8_t min = 0;
    index += (isUp ? -1 : 1);
    if(index > max) {
      index = (isUp ? max : min);
    }
    return index;
}

static MessageScreen* message_screen;
static MapScreen* map_screen;
static ConfigScreen* config_screen;
static MenuScreen* menu_screen;

#include "configscreen.c"
#include "mapscreen.c"
#include "messagescreen.c"
#include "menuscreen.c"

typedef enum {
    MAP_SCREEN,
    MESSAGE_SCREEN,
    CONFIG_SCREEN,
    MENU_SCREEN
} screen_t;

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
    return MAP_SCREEN;
}

void initScreens() {

  map_screen = createMapScreen();
  message_screen = createMessageScreen();
  config_screen = createConfigScreen();
  menu_screen = createMenuScreen();

  lv_scr_load(menu_screen->root);
}

#endif