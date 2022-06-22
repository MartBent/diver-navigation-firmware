#include "esp_system.h"

uint32_t getSecondsSinceStart() {
    return esp_timer_get_time() / 1000000;
}