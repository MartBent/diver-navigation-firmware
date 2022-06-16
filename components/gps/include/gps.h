#ifndef GPS_H
#define GPS_H

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/uart.h"
#include "nmea.h"
#include "gpgll.h"
#include "gpgga.h"
#include "gprmc.h"
#include "gpgsa.h"
#include "gpvtg.h"
#include "gptxt.h"
#include "gpgsv.h"


typedef struct {
    double latitude;
    double longitude;
} GPSModuleCoordinates;


static const uart_port_t gps_uart_num = UART_NUM_1;
static const int uart_buffer_size = (1024);
static const int U2_TX = 0;
static const int U2_RX = 4;

void setup_gps();
GPSModuleCoordinates* read_gps_coordinates();


#endif //GPS_H