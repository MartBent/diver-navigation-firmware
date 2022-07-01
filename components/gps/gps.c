#ifndef GPS_C
#define GPS_C

#include "gps.h"

static void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    double* data = (double*)event_data;
    newestLatitude = data[0];
    newestLongtitude = data[1];
}

void setup_gps(esp_event_handler_t event_handler)
{
    /* NMEA parser configuration */
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    /* init NMEA parser library */
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    /* register event handler  for NMEA parser library */
    
    nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);
    
}

//Returns map center for now.
void read_gps_coordinates(GPSModuleCoordinates* coordinates){
    memcpy(&coordinates->latitude, &newestLatitude, sizeof(double));
    memcpy(&coordinates->longtitude, &newestLongtitude, sizeof(double));
}

#endif