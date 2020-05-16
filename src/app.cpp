#include "app.h"

#include "esp_common.h"
#include "gpio.h"

static void task_blink(void* ignore)
{
    while(true) {
        GPIO_OUTPUT_SET(2, 0);
        vTaskDelay(100 / portTICK_RATE_MS);
        GPIO_OUTPUT_SET(2, 1);
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    vTaskDelete(nullptr);
}

void init_app() {
    xTaskCreate(&task_blink, (const signed char*)"startup", 2048, NULL, 1, NULL);
}
