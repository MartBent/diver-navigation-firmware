#ifndef GPS_H
#define GPS_H

#include <string.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "nmea_parser.h"

typedef struct {
    double latitude;
    double longtitude;
} GPSModuleCoordinates;

static double newestLatitude = 0;
static double newestLongtitude = 0;

static const uart_port_t gps_uart_num = UART_NUM_1;
static const int uart_buffer_size = (1024);

void setup_gps();
void read_gps_coordinates(GPSModuleCoordinates* coordinates);

#endif //GPS_H