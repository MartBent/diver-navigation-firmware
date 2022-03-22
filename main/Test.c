#include <stdio.h>
#include <lmic.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rst = 14,
  .dio = { 26, 34, 35 },
  .spi = { /* MISO = */ 19, /* MOSI = */ 27, /* SCK = */ 5 },
  .rxtx = LMIC_UNUSED_PIN,
};

void hello_task(void *pvParameter)
{
    printf("Hello world");
}
void lmic_loop(void *pvParameter)
{
    os_runloop_once();
}


void app_main(void)
{
    // LMIC init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();

    xTaskCreate(&hello_task, "hello_task", 2048, NULL, 5, NULL);
    //xTaskCreate(&lmic_loop, "lmic_task", 2048, NULL, 5, NULL);
}
