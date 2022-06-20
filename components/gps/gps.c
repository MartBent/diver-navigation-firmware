#include "gps.h"

void setup_gps(esp_event_handler_t event_handler)
{
    /* NMEA parser configuration */
    nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();
    /* init NMEA parser library */
    nmea_parser_handle_t nmea_hdl = nmea_parser_init(&config);
    /* register event handler  for NMEA parser library */
    if(event_handler != NULL) {
        nmea_parser_add_handler(nmea_hdl, event_handler, NULL);
    }
}

void read_gps_coordinates(GPSModuleCoordinates* coordinates){
    coordinates->latitude = 52.219591;//newestLatitude;
    coordinates->longtitude = 6.880593;//newestLongtitude;
}