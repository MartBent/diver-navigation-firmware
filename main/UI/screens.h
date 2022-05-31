#ifndef SCREENS_H
#define SCREENS_H

#include "lvgl.h"
#include "image.c"
#include "font.c"

typedef struct {
    lv_obj_t* root;
    lv_obj_t* map;
    lv_obj_t* btn_ok;
    lv_obj_t* btn_call;
    lv_obj_t* btn_messages;
    lv_obj_t* btn_config;
} MapScreen;

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

lv_obj_t* getButton(lv_obj_t* parent, char* text, lv_color_t color);

MenuScreen* createMenuScreen();
void handleMenuScreenButton(uint8_t button_num);

MapScreen* createMapScreen();
void handleMapScreenButton(uint8_t button_num);

MessageScreen* createMessageScreen();
void handleMessageScreenButton(uint8_t button_num);

ConfigScreen* createConfigScreen();
void handleConfigScreenButton(uint8_t button_num);

#endif