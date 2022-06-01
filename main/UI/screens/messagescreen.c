#include "../../Lora/Lora.h"
#include <stdio.h>

MessageScreen* createMessageScreen() {
  MessageScreen* message_screen = (MessageScreen*) malloc(sizeof(MessageScreen));
  message_screen->root = lv_obj_create(NULL, NULL);
  message_screen->label = lv_label_create(message_screen->root, NULL);
  lv_label_set_text(message_screen->label, "Messages screen");
  return message_screen;
}

void handleMessageScreenButton(uint8_t button_num) {
    if(button_num == 1) {
        printf("Sending lora from message screen\n");
        uint8_t data[3] = {1,2,3};
        lora_send_bytes(data,3);
    }
}