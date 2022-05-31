#ifndef SCREENS_C
#define SCREENS_C

#include "screens.h"

lv_obj_t* getButton(lv_obj_t* parent, char* text, lv_color_t color) {

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

MapScreen* createMapScreen() {
  MapScreen* map_screen = malloc(sizeof(MapScreen));

  map_screen->root = lv_obj_create(NULL, NULL);

  map_screen->map = lv_img_create(map_screen->root, NULL);
  lv_img_set_src(map_screen->map, &hilgelo);
  lv_obj_align(map_screen->map, NULL, LV_ALIGN_CENTER, 0, 0);

  map_screen->btn_ok = getButton(map_screen->root, "OK", LV_COLOR_GREEN);
  lv_obj_align(map_screen->btn_ok, NULL, LV_ALIGN_IN_TOP_LEFT, 3, 3);

  map_screen->btn_call  = getButton(map_screen->root, "Call", LV_COLOR_RED);
  lv_obj_align(map_screen->btn_call, NULL, LV_ALIGN_IN_TOP_LEFT, 43, 3);

  map_screen->btn_config = getButton(map_screen->root, "Settings", LV_COLOR_BLUE);
  lv_obj_align(map_screen->btn_config, NULL, LV_ALIGN_IN_TOP_LEFT, 83, 3);

  map_screen->btn_messages = getButton(map_screen->root, "Messaging", LV_COLOR_YELLOW);
  lv_obj_align(map_screen->btn_messages, NULL, LV_ALIGN_IN_TOP_LEFT, 123, 3);

  return map_screen;
}

MessageScreen* createMessageScreen() {
  MessageScreen* message_screen = (MessageScreen*) malloc(sizeof(MessageScreen));
  message_screen->root = lv_obj_create(NULL, NULL);
  message_screen->label = lv_label_create(message_screen->root, NULL);
  lv_label_set_text(message_screen->label, "Messages screen");
  return message_screen;
}

ConfigScreen* createConfigScreen() {
  ConfigScreen* config_screen = (ConfigScreen*) malloc(sizeof(ConfigScreen));
  config_screen->root = lv_obj_create(NULL, NULL);

  config_screen->btn_up  = getButton(config_screen->root, "/\\", LV_COLOR_YELLOW);
  lv_obj_align(config_screen->btn_up, NULL, LV_ALIGN_IN_TOP_LEFT, 8, 3);

  config_screen->btn_down  = getButton(config_screen->root, "\\/", LV_COLOR_BLUE);
  lv_obj_align(config_screen->btn_down, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 8, 0);

  config_screen->label = lv_label_create(config_screen->root, NULL);
  lv_label_set_text(config_screen->label, "Config screen");
  lv_obj_align(config_screen->label, NULL, LV_ALIGN_CENTER, 0, 0);

  return config_screen;
}

MenuScreen* createMenuScreen() {
  MenuScreen* menu_screen = (MenuScreen*) malloc(sizeof(MenuScreen));
  menu_screen->current_option = 0;
  menu_screen->root = lv_obj_create(NULL, NULL);
  char* options[4] = {"Map", "Messages", "Stats", "Config"};
  for(int i = 0; i < 4; i++) {
    menu_screen->menuOptions[i] = getButton(menu_screen->root, options[i], LV_COLOR_WHITE);
    lv_obj_align(menu_screen->menuOptions[i], NULL, LV_ALIGN_IN_TOP_LEFT, 5, i*25);
  }

  lv_obj_set_style_local_bg_color(menu_screen->menuOptions[0], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  return menu_screen;
}

#endif