#include "lvgl.h"
#include "../font.c"
#include "../../stats/time.c"

static bool isDiving = false;
static bool isSyncing = false;

static uint8_t* compressed_map;
static uint16_t compressed_map_index = 0;

static uint16_t compressed_length = 0;
static double rx_latitude = 0;
static double rx_longtitude = 0;
static uint16_t frame_amount = 0;

button_t* getMenuButton(lv_obj_t* parent, char* text, lv_color_t color) {

  button_t* button = malloc(sizeof(button_t));

  button->back = lv_obj_create(parent, NULL);
  lv_obj_set_width(button->back, 50);
	lv_obj_set_height(button->back, 20);
	button->label = lv_label_create(button->back, NULL);
	lv_label_set_text(button->label, text);
  
	lv_obj_set_style_local_text_color(button->label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK );
  lv_obj_set_style_local_text_font(button->label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_8);
	lv_obj_set_style_local_bg_color(button->back, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, color);
  lv_obj_align(button->label, NULL, LV_ALIGN_CENTER, 0, 0);
  return button;
}

MenuScreen* createMenuScreen() {
  MenuScreen* menu_screen = (MenuScreen*) malloc(sizeof(MenuScreen));
  menu_screen->current_option = 0;
  menu_screen->root = lv_obj_create(NULL, NULL);
  char* options[4] = {"Map", "Messages", "Not sync", "Start"};
  for(int i = 0; i < 4; i++) {
    menu_screen->menu_options[i] = getMenuButton(menu_screen->root, options[i], LV_COLOR_WHITE);
    lv_obj_align(menu_screen->menu_options[i]->back, NULL, LV_ALIGN_IN_TOP_LEFT, 5, i*25+10);
  }
  lv_obj_set_style_local_bg_color(menu_screen->menu_options[0]->back, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);

  menu_screen->lbl_time = lv_label_create(menu_screen->root, NULL);
  lv_label_set_text(menu_screen->lbl_time, "Time:");
  lv_obj_align(menu_screen->lbl_time, NULL, LV_ALIGN_IN_TOP_LEFT, 55, 10);

  menu_screen->lbl_depth = lv_label_create(menu_screen->root, NULL);
  lv_label_set_text(menu_screen->lbl_depth, "Depth:");
  lv_obj_align(menu_screen->lbl_depth, NULL, LV_ALIGN_IN_TOP_LEFT, 55, 35);

  menu_screen->lbl_battery = lv_label_create(menu_screen->root, NULL);
  lv_label_set_text(menu_screen->lbl_battery, "Battery:");
  lv_obj_align(menu_screen->lbl_battery, NULL, LV_ALIGN_IN_TOP_LEFT, 55, 60);

  return menu_screen;
}

void handleMenuScreenButton(uint8_t button_num) {
  switch (button_num) {
    case 1: {
      switch(menu_screen->current_option) {
        case 0: {
          lv_scr_load(map_screen->root);
          break;
        }
        case 1: 
          lv_scr_load(message_screen->root);
          break;
        case 2: 
          if(isSyncing) {
            isSyncing = false;
            compressed_length = 0;
            compressed_map_index = 0;
            frame_amount = 0;
            rx_latitude = 0;
            rx_longtitude = 0;

            //Set back the button text
            lv_label_set_text(menu_screen->menu_options[2]->label, "Not Sync");
          }
          break;
        case 3:
          if(!isDiving) {
            lv_label_set_text(menu_screen->menu_options[3]->label, "Stop");
            uint8_t bytes[2] = {3, 1};
            lora_send_bytes(bytes, 2);
            isDiving = true;
          } else {
            uint8_t bytes[2] = {3, 0};
            lora_send_bytes(bytes, 2);
            lv_label_set_text(menu_screen->menu_options[3]->label, "Start");
            isDiving = false;
          }
          
          break;
      }
      break;
    }
    
    case 2:
    case 3: 
    {
      for(int i = 0; i < 4; i++) 
      {
        lv_obj_set_style_local_bg_color(menu_screen->menu_options[i]->back, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
      } 
    
      menu_screen->current_option = scrollMenu(menu_screen->current_option, 4, button_num == 3);

      lv_obj_set_style_local_bg_color(menu_screen->menu_options[menu_screen->current_option]->back, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
      break;

    }
  }
}