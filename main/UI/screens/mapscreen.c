
#include "../image.c"
#include "../../stats/location.c"
#include <stdio.h>

MapScreen* createMapScreen() {
  MapScreen* map_screen = malloc(sizeof(MapScreen));

  map_screen->root = lv_obj_create(NULL, NULL);

  map_screen->map = lv_img_create(map_screen->root, NULL);

  lv_img_set_src(map_screen->map, &hilgelo);
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