#include "lvgl.h"
#include "../font.c"

lv_obj_t* getConfigScreenButton(lv_obj_t* parent, char* text, lv_color_t color) {

  lv_obj_t* bkgrnd = lv_obj_create(parent, NULL);
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

ConfigScreen* createConfigScreen() {
  ConfigScreen* config_screen = (ConfigScreen*) malloc(sizeof(ConfigScreen));
  config_screen->root = lv_obj_create(NULL, NULL);

  config_screen->btn_up  = getConfigScreenButton(config_screen->root, "/\\", LV_COLOR_YELLOW);
  lv_obj_align(config_screen->btn_up, NULL, LV_ALIGN_IN_TOP_LEFT, 8, 3);

  config_screen->btn_down  = getConfigScreenButton(config_screen->root, "\\/", LV_COLOR_BLUE);
  lv_obj_align(config_screen->btn_down, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 8, 0);

  config_screen->label = lv_label_create(config_screen->root, NULL);
  lv_label_set_text(config_screen->label, "Config screen");
  lv_obj_align(config_screen->label, NULL, LV_ALIGN_CENTER, 0, 0);

  return config_screen;
}

void handleConfigScreenButton(uint8_t button_num) {
    
}