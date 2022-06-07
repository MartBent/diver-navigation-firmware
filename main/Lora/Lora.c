#ifndef LORA_C
#define LORA_C

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "driver/uart.h"

typedef struct {
    float latitude;
    float longitude;
} GpsMessage;

typedef struct {
    uint8_t length;
    char message[128];
} CommunicationMessage;

typedef struct {
    uint8_t length;
    uint8_t* data;
    uint8_t index; //The map message might have to be cut into seperate messages.
} MapMessage;

static const uart_port_t uart_num = UART_NUM_2;


void setup_lora() {
  uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122, 
  };

  // Configure UART parameters
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

  ESP_ERROR_CHECK(uart_set_pin(uart_num, 17, 16, 0, 0));

  // Setup UART buffered IO with event queue
  const int uart_buffer_size = (1024 * 2);
  QueueHandle_t uart_queue;
  
  ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
}

void lora_send_bytes(uint8_t* tx_data, uint8_t length) {
    uart_write_bytes(uart_num, tx_data, length);
}

void lora_send_chars(char* tx_data, uint8_t length) {
    uart_write_bytes(uart_num, tx_data, length);
}

uint8_t lora_receive(uint8_t* rx_data) {
    int length = 0;
    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));
    if(length > 0 && length < 128) {
      length = uart_read_bytes(uart_num, rx_data, length, 100); 
    }
    return length;
}

GpsMessage* decodeGpsMessage(uint8_t* data, uint8_t length) {
  GpsMessage* msg = malloc(sizeof(GpsMessage));
  msg->latitude = (float)data[2];
  msg->longitude = (float)data[1];
  return msg;
}

CommunicationMessage* decodeCommMessage(uint8_t* data, uint8_t length) {
  CommunicationMessage* msg = malloc(sizeof(CommunicationMessage));
  msg->length = data[1];
  for(int i = 0; i < msg->length; i++) {
    msg->message[0] = data[i+2];
  }
  msg->message[msg->length] = '\0';
  return msg;
}

MapMessage* decodeMapMessage(uint8_t* data, uint8_t length) {
  MapMessage* msg = malloc(sizeof(MapMessage));
  return msg;
}

#endif