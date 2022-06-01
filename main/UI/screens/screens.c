#ifndef SCREENS_C
#define SCREENS_C

#include "lvgl.h"

typedef struct {
    lv_obj_t* root;
    lv_obj_t* label;
} MessageScreen;

typedef struct  {
    lv_obj_t* root;
    lv_obj_t* label;
    lv_obj_t* btn_up;
    lv_obj_t* btn_down;
} ConfigScreen;

typedef struct  {
    lv_obj_t* root;
    lv_obj_t* menuOptions[6];
    lv_obj_t* btn_up;
    lv_obj_t* btn_down;
    uint8_t current_option;
} MenuScreen;

typedef struct {
    lv_obj_t* root;
    lv_obj_t* map;
    lv_obj_t* location_marker;
} MapScreen;

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
    STATS_SCREEN,
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
    if(lv_scr_act() == map_screen->root) {
        return STATS_SCREEN;
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