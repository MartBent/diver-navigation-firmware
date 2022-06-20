#include "../../Lora/Lora.c"
#include <stdio.h>
#include "../../stats/depth.c"

char* message_options_str[5] = {"I am OK.\0", "I am in trouble.\0","I see a shark.\0", "My location is {}.\0", "My depth is %dm.\0"};

lv_obj_t* getMessageButton(lv_obj_t* parent, char* text, lv_color_t color) {

  lv_obj_t* bkgrnd = lv_obj_create(parent, NULL);
  lv_obj_set_width(bkgrnd, 75);
	lv_obj_set_height(bkgrnd, 20);
	lv_obj_t* label = lv_label_create(bkgrnd, NULL);
	lv_label_set_text(label, text);
  
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK );
  lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_8);
	lv_obj_set_style_local_bg_color(bkgrnd, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, color);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
  return bkgrnd;
}
lv_obj_t* getControlButton(lv_obj_t* parent, char* text, lv_color_t color) {

  lv_obj_t* bkgrnd = lv_obj_create(parent, NULL);
  lv_obj_set_width(bkgrnd, 24);
	lv_obj_set_height(bkgrnd, 20);
	lv_obj_t* label = lv_label_create(bkgrnd, NULL);
	lv_label_set_text(label, text);
  
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK );
  lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_8);
	lv_obj_set_style_local_bg_color(bkgrnd, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, color);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
  return bkgrnd;
}
MessageScreen* createMessageScreen() {
  MessageScreen* message_screen = (MessageScreen*) malloc(sizeof(MessageScreen));
  message_screen->current_option = 0;
  message_screen->root = lv_obj_create(NULL, NULL);

  //Message log
  message_screen->message_box = lv_textarea_create(message_screen->root, NULL);

  lv_obj_set_width(message_screen->message_box, 80);
	lv_obj_set_height(message_screen->message_box, 128 - 30);

  lv_obj_align(message_screen->message_box, NULL, LV_ALIGN_IN_TOP_LEFT, 80, 3);

  lv_obj_set_style_local_text_font(message_screen->message_box, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_8);

  lv_textarea_set_cursor_hidden(message_screen->message_box, true);
  lv_textarea_set_text(message_screen->message_box, "\0");

  //Message buttons
  for(int i = 0; i < 5; i++) {
    message_screen->message_options[i] = getMessageButton(message_screen->root, message_options_str[i], LV_COLOR_WHITE);
    lv_obj_align(message_screen->message_options[i], NULL, LV_ALIGN_IN_TOP_LEFT, 3, i*25+3);
  }
  lv_obj_set_style_local_bg_color(message_screen->message_options[0], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);

  //Control buttons
  message_screen->btn_up = getControlButton(message_screen->root, "/\\", LV_COLOR_YELLOW);
  lv_obj_align(message_screen->btn_up, NULL, LV_ALIGN_IN_TOP_LEFT, 80, 103);

  message_screen->btn_down = getControlButton(message_screen->root, "\\/", LV_COLOR_BLUE);
  lv_obj_align(message_screen->btn_down, NULL, LV_ALIGN_IN_TOP_LEFT, 83+24, 103);

  message_screen->btn_clear = getControlButton(message_screen->root, "Menu", LV_COLOR_LIME);
  lv_obj_align(message_screen->btn_clear, NULL, LV_ALIGN_IN_TOP_LEFT, 86+48, 103);
  return message_screen;
}

void handleMessageScreenButton(uint8_t button_num) {
    switch (button_num) {
      case 1: {
        if(message_screen->current_option == 2) {
          char* message = malloc(strlen(message_options_str[message_screen->current_option]) + 1);
          message[0] = 0;
          sprintf(message+1, message_options_str[message_screen->current_option], getCurrentDepth());
          printf(message);

          lora_send_chars(message, strlen(message_options_str[message_screen->current_option])+1);
          lv_textarea_add_text(message_screen->message_box, "Tx: \0");
          lv_textarea_add_text(message_screen->message_box, message);
          lv_textarea_add_char(message_screen->message_box, '\n');
          free(message);

        } else if(message_screen->current_option == 3) {

          char* message = malloc(strlen(message_options_str[message_screen->current_option]) + 1);
          message[0] = 0;
          sprintf(message+1, message_options_str[message_screen->current_option], getCurrentDepth());
          printf(message);

          lora_send_chars(message, strlen(message_options_str[message_screen->current_option])+1);
          lv_textarea_add_text(message_screen->message_box, "Tx: \0");
          lv_textarea_add_text(message_screen->message_box, message);
          lv_textarea_add_char(message_screen->message_box, '\n');
          free(message);

        } else {
          char* message = malloc(strlen(message_options_str[message_screen->current_option]) + 1);
          message[0] = 0;
          memcpy(message+1, message_options_str[message_screen->current_option], strlen(message_options_str[message_screen->current_option]));
          printf(message);

          lora_send_chars(message, strlen(message_options_str[message_screen->current_option])+1);
          lv_textarea_add_text(message_screen->message_box, "Tx: \0");
          lv_textarea_add_text(message_screen->message_box, message_options_str[message_screen->current_option]);
          lv_textarea_add_char(message_screen->message_box, '\n');
          free(message);
        }

        
        break;
      }
      case 2:
      case 3:
        for(int i = 0; i < 5; i++) 
        {
          lv_obj_set_style_local_bg_color(message_screen->message_options[i], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        }
        message_screen->current_option = scrollMenu(message_screen->current_option, 5, button_num == 3);
        lv_obj_set_style_local_bg_color(message_screen->message_options[message_screen->current_option], LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
        break;
      case 4:
        lv_scr_load(menu_screen->root);
        break;
    }
}

void processCommunicationMessage(const CommunicationMessage* message) {
  lv_textarea_add_text(message_screen->message_box, "Rx: \0");
  lv_textarea_add_text(message_screen->message_box, message->message);
  lv_textarea_add_char(message_screen->message_box, '\n');
}