
typedef struct {
    double latitude;
    double longtitude;
} location_t;

location_t getCurrentLocation() {
    location_t location = {
        .latitude = 51.992901, 
        .longtitude = 6.719517
    };
    return location;
}

location_t getCurrentMapCenterLocation() {  
    location_t location = {
        .latitude = 51.9934953,
        .longtitude = 6.7187879
    };
    return location;
}

void locationToPixels(location_t location, uint8_t* x, uint8_t* y) {

    double lati_per_pixel = 0.00005140625;
    double long_per_pixel = 0.0000840875;

    //Convert coordinates to pixel on map
    location_t map_center = getCurrentMapCenterLocation();

    double delta_lati = map_center.latitude - location.latitude;
    double delta_long = map_center.longtitude - location.longtitude;

    int map_x = delta_long / long_per_pixel;
    int map_y = delta_lati / lati_per_pixel;

    *x = map_x;
    *y = map_y;
}
