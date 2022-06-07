#include "esp_system.h"
#include "esp_log.h"

uint32_t getSecondsSinceStart() {
    return esp_timer_get_time() / 1000000;
}