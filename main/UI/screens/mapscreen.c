
#ifndef MAPSCREEN_C
#define MAPSCREEN_C

#include <stdio.h>
#include "esp_system.h"
#include "esp_partition.h"
#include "../../Lora/Lora.c"
#include <zlib.h>

uint8_t* map;
lv_img_dsc_t* map_src;

static double center_latitude = 0;
static double center_longtitude = 0;

static const esp_partition_t* part_map;

void retrieveMap(uint8_t* map_out, double* latitude, double* longtitude) {
  ESP_ERROR_CHECK(esp_partition_read_raw(part_map, 0, map_out, 40960));
  ESP_ERROR_CHECK(esp_partition_read_raw(part_map, 40960, latitude, 8));
  ESP_ERROR_CHECK(esp_partition_read_raw(part_map, 40968, longtitude, 8));
}

void saveMap(const uint8_t* map_in, const double latitude, const double  longtitude) {
  ESP_ERROR_CHECK(esp_partition_erase_range(part_map, 0, 40960+4096));

  ESP_ERROR_CHECK(esp_partition_write_raw(part_map, 0, map_in, 40960));
  ESP_ERROR_CHECK(esp_partition_write_raw(part_map, 40960, &latitude, 8));
  ESP_ERROR_CHECK(esp_partition_write_raw(part_map, 40968, &longtitude, 8));
}

MapScreen* createMapScreen() {
  
  MapScreen* map_screen = malloc(sizeof(MapScreen));

  part_map = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "map");

  map_src = malloc(sizeof(lv_img_dsc_t));
  map = malloc(40960);

  retrieveMap(map, &center_latitude, &center_longtitude);

  printf("Map center: %0.5f, %0.5f", center_latitude, center_longtitude);

  map_src->header.cf = LV_IMG_CF_TRUE_COLOR;
  map_src->header.always_zero = 0;
  map_src->header.reserved = 0;
  map_src->header.w = 160;
  map_src->header.h = 128;
  map_src->data_size = 40960;
  map_src->data = map;

  map_screen->root = lv_obj_create(NULL, NULL);

  map_screen->map = lv_img_create(map_screen->root, NULL);

  lv_img_set_src(map_screen->map, map_src);
  lv_obj_align(map_screen->map, NULL, LV_ALIGN_CENTER, 0, 0);

  map_screen->location_marker = lv_label_create(map_screen->root, NULL);
  lv_label_set_text(map_screen->location_marker, "X");
  lv_obj_align(map_screen->location_marker, NULL, LV_ALIGN_CENTER, 255, 255);

  map_screen->location_boat_marker = lv_label_create(map_screen->root, NULL);
  lv_label_set_text(map_screen->location_boat_marker, "O");
  lv_obj_align(map_screen->location_boat_marker, NULL, LV_ALIGN_CENTER, 255, 255);

  return map_screen;
}

void handleMapScreenButton(uint8_t button_num) {
    switch (button_num) {
      case 1:
      case 2:
      case 3:
        break;
      case 4: {
          lv_scr_load(menu_screen->root);
          break;
        }
    }
}

typedef struct {
    double latitude;
    double longtitude;
} location_t;

void locationToPixels(double latitude, double longtitude, int* x, int* y) {

    double lati_per_pixel = 0.00005140625;
    double long_per_pixel = 0.0000840875;

    double delta_lati = center_latitude - latitude;
    double delta_long = center_longtitude - longtitude;

    int map_x = delta_long / long_per_pixel;
    int map_y = delta_lati / lati_per_pixel;

    *x = map_x;
    *y = map_y;
}

void processGpsMessage(const GpsMessage* msg, bool isBoat) {
  int x = 0;
  int y = 0;
  
  locationToPixels(msg->latitude, msg->longitude, &x , &y);

  if(isBoat) {
    lv_obj_align(map_screen->location_boat_marker, NULL, LV_ALIGN_CENTER, -x, y);
  } else {
    lv_obj_t* point = lv_label_create(map_screen->root, NULL);
    lv_label_set_text(point, "+");
    lv_obj_align(point, NULL, LV_ALIGN_CENTER, -x, y);
  }
}

void adjustLocationMarker(double latitude, double longtitude) {

  int x = 0;
  int y = 0;
  locationToPixels(latitude, longtitude, &x , &y);
  lv_obj_align(map_screen->location_marker, NULL, LV_ALIGN_CENTER, -x, y);
}

#endif

