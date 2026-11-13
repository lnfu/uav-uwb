#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "main";

void app_main(void) {
    for (unsigned n = 0;; n++) {
        ESP_LOGI(TAG, "uav_uwb firmware alive %u", n);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
