#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "driver/uart.h"

static const uart_port_t uart_num = UART_NUM_2;
static uint8_t bufferedData[128] = {};
static uint8_t bufferedDataLenth = 0;

void setup_lora();
void lora_send(uint8_t* tx_data, uint8_t length);
uint8_t lora_receive(uint8_t* rx_data);