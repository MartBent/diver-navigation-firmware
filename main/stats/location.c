
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
