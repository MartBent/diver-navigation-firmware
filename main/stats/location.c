
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
        .latitude = 52.01186,
        .longtitude = 6.70157
    };
    return location;
}

void locationToPixels(double latitude, double longtitude, int* x, int* y) {

    double lati_per_pixel = 0.00005140625;
    double long_per_pixel = 0.0000840875;

    //Convert coordinates to pixel on map
    location_t map_center = getCurrentMapCenterLocation();

    double delta_lati = map_center.latitude - latitude;
    double delta_long = map_center.longtitude - longtitude;

    int map_x = delta_long / long_per_pixel;
    int map_y = delta_lati / lati_per_pixel;

    *x = map_x;
    *y = map_y;
}
