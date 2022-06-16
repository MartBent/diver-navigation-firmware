#include "gps.h"

void setup_gps(){
      
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122, 
    };

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(gps_uart_num, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(gps_uart_num, U2_TX, U2_RX, 0, 0));

    // Setup UART buffered IO with event queue

    QueueHandle_t uart_queue;
    
    ESP_ERROR_CHECK(uart_driver_install(gps_uart_num, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
}

GPSModuleCoordinates* read_gps_coordinates(){
    
    GPSModuleCoordinates *coord = NULL;

    // Configure a temporary buffer for the incoming data
    char *buffer = (char*) malloc(uart_buffer_size + 1);
    size_t total_bytes = 0;

    // Read data from the UART
    total_bytes = uart_read_bytes(gps_uart_num, (uint8_t*) buffer , uart_buffer_size, 100 / portTICK_PERIOD_MS);
    uart_flush(gps_uart_num);
    
    if (total_bytes > 0) {

        while(1) {

            nmea_s *data;
            char *end;

            /* find start (a dollar sign) */
            char* start = memchr(buffer, '$', total_bytes);
            if (start == NULL) {
                total_bytes = 0;
                break;
            }

            /* find end of line */
            end = memchr(start, '\r', total_bytes - (start - buffer));
            if (NULL == end || '\n' != *(++end)) {
                break;
            }
            
            start[2] = 'P'; //why other devices ID aren't supported??!?!?!?!?! WHO KNOWS?! Fixed in this way
            end[-1] = NMEA_END_CHAR_1;
            end[0] = NMEA_END_CHAR_2;

            /* handle data */
            data = nmea_parse(start, end - start + 1, 0);
            if (data == NULL) {
                ESP_LOGI("GPS","Failed to parse the sentence!\n");
                ESP_LOGI("","  Type: %.5s (%d)\n", start+1, nmea_get_type(start));
            } else if(data->errors == 0) {

                if (NMEA_GPGLL == data->type) {
                    nmea_gpgll_s *pos = (nmea_gpgll_s *) data;
                    coord = malloc(sizeof(GPSModuleCoordinates));
                    coord->latitude = pos->longitude.minutes;
                    coord->longitude = pos->latitude.minutes;
                } else if (NMEA_GPRMC == data->type) {
                    nmea_gprmc_s *pos = (nmea_gprmc_s *) data;                    
                    coord = malloc(sizeof(GPSModuleCoordinates));
                    coord->latitude = pos->longitude.minutes;
                    coord->longitude = pos->latitude.minutes;
                }

                nmea_free(data);
            }

            if (end == buffer + total_bytes) {
                total_bytes = 0;
                break;
            }

            /* copy rest of buffer to beginning */
            if (buffer != memmove(buffer, end, total_bytes - (end - buffer))) {
                total_bytes = 0;
                break;
            }

            total_bytes -= end - buffer;

        };

        free(buffer);
    }

    return coord;
}