#include "lvgl.h"

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
} ConfigScreen;

lv_obj_t* getButton(lv_obj_t* parent, char* text, lv_color_t color);
MapScreen* createMapScreen();
MessageScreen* createMessageScreen();
ConfigScreen* createConfigScreen();