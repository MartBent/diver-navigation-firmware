
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
  //lv_obj_align(map_screen->location_marker, NULL, LV_ALIGN_CENTER, 0, 0);

  return map_screen;
}

void handleMapScreenButton(uint8_t button_num) {

    double lati_per_pixel = 0.00005140625;
    double long_per_pixel = 0.0000840875;

    location_t location = getCurrentLocation();
    
    //Convert coordinates to pixel on map
    location_t map_center = getCurrentMapCenterLocation();

    double delta_lati = map_center.latitude - location.latitude;
    double delta_long = map_center.longtitude - location.longtitude;

    printf("Deltas lati: %.10f, Long: %.10f", delta_lati, delta_long);

    int map_x = delta_long / long_per_pixel;
    int map_y = delta_lati / lati_per_pixel;

    printf("Adjustic marker: %d %d", map_x, map_y);
    lv_obj_align(map_screen->location_marker, NULL, LV_ALIGN_CENTER, -map_x, map_y);
}