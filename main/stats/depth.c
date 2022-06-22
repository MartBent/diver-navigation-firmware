#ifndef DEPTH_C
#define DEPTH_C

#include <esp_random.h>

uint8_t getCurrentDepth() {
    return esp_random() % 20;
}

#endif