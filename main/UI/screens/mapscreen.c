
#include "../../stats/location.c"
#include <stdio.h>
#include "esp_system.h"
#include "esp_partition.h"
#include "../../Lora/Lora.c"
uint8_t* map;
lv_img_dsc_t* map_src;

MapScreen* createMapScreen() {
  
  MapScreen* map_screen = malloc(sizeof(MapScreen));

  map_src = malloc(sizeof(lv_img_dsc_t));
  map = malloc(40960);

  map_src->header.cf = LV_IMG_CF_TRUE_COLOR;
  map_src->header.always_zero = 0;
  map_src->header.reserved = 0;
  map_src->header.w = 160;
  map_src->header.h = 128;
  map_src->data_size = 20480 * LV_COLOR_SIZE / 32;
  map_src->data = map;

  map_screen->root = lv_obj_create(NULL, NULL);

  map_screen->map = lv_img_create(map_screen->root, NULL);

  const esp_partition_t* part  = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, "storage");

  ESP_ERROR_CHECK(esp_partition_read_raw(part, 0, map, 40960));

  lv_img_set_src(map_screen->map, map_src);
  lv_obj_align(map_screen->map, NULL, LV_ALIGN_CENTER, 0, 0);

  map_screen->location_marker = lv_label_create(map_screen->root, NULL);
  lv_label_set_text(map_screen->location_marker, "X");

  return map_screen;
}

void handleMapScreenButton(uint8_t button_num) {
    switch (button_num) {
      case 1: {
          location_t loc = getCurrentMapCenterLocation();
          uint8_t x = 0;
          uint8_t y = 0;
          locationToPixels(loc, &x , &y);
          lv_obj_align(map_screen->location_marker, NULL, LV_ALIGN_CENTER, -x, y);
          break;
        }
      case 2:
      case 3:
        break;
      case 4: {
          lv_scr_load(menu_screen->root);
          break;
        }
    }
}

void processGpsMessage(GpsMessage* msg) {
  //printf("Lati: %.5f\n", msg->latitude);
  //printf("Long: %.5f\n", msg->longitude);
}