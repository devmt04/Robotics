#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "sdkconfig.h"

#include "common/common.h"
#include "Engine/engine.h"
// use ESP_ERROR_CHECK()


void app_main(void)
{
    // xTaskCreate();
    esp_log_level_set("*", ESP_LOG_NONE);
    engine_init();
    while(1){
        delayMS(100);
        // taskYIELD();  // yields immediately to other tasks
    }
}