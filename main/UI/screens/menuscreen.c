#include "lvgl.h"
#include "../font.c"

lv_obj_t* getMenuButton(lv_obj_t* parent, char* text, lv_color_t color) {

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

MenuScreen* createMenuScreen() {
  MenuScreen* menu_screen = (MenuScreen*) malloc(sizeof(MenuScreen));
  menu_screen->current_option = 0;
  menu_screen->root = lv_obj_create(NULL, NULL);
  char* options[4] = {"Map", "Messages", "Stats", "Config"};
  for(int i = 0; i < 4; i++) {
    menu_screen->menu_options[i] = getMenuButton(menu_screen->root, options[i], LV_COLOR_WHITE);
    lv_obj_align(menu_screen->menu_options[i], NULL, LV_ALIGN_IN_TOP_LEFT, 5, i*25);
  }

  lv_obj_set_style_local_bg_color(menu_screen->menu_options[0], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  return menu_screen;
}

void handleMenuScreenButton(uint8_t button_num) {
  switch (button_num) {
    case 1: {
      switch(menu_screen->current_option) {
        case 0: 
          lv_scr_load(map_screen->root);
          break;
        case 1: 
          lv_scr_load(message_screen->root);
          break;
        case 2: 
          lv_scr_load(stats_screen->root);
          break;
        case 3: 
          lv_scr_load(config_screen->root);
          break;
      }
      break;
    }
    
    case 2:
    case 3: 
    {
      for(int i = 0; i < 4; i++) 
      {
        lv_obj_set_style_local_bg_color(menu_screen->menu_options[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
      } 
    
      menu_screen->current_option = scrollMenu(menu_screen->current_option, 4, button_num == 3);

      lv_obj_set_style_local_bg_color(menu_screen->menu_options[menu_screen->current_option], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
      break;

    }
  }
}