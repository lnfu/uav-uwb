#include "spi_test.h"

#include <inttypes.h>
#include <stdbool.h>

#include "deca_device_api.h"
#include "deca_regs.h"
#include "dw1000_port.h"
#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ITERATIONS 2000

// Values after a reset. SYS_CFG and TX_FCTRL are not zero and have a non-zero
// header, so a bit shift on MOSI or on MISO changes the result.
#define SYS_CFG_RESET 0x00001200
#define TX_FCTRL_RESET 0x0015C00C

static const char *TAG = "spi_test";

static void check_reg(const char *name, uint32_t actual, uint32_t expected) {
    if (actual == expected) {
        ESP_LOGI(TAG, "%-8s = 0x%08" PRIX32 " ok", name, actual);
    } else {
        ESP_LOGE(TAG, "%-8s = 0x%08" PRIX32 ", expected 0x%08" PRIX32, name, actual, expected);
    }
}

// Reads and writes registers through the driver at one SPI clock rate.
static void stress(int hz) {
    dw1000_port_set_rate(hz);
    int id_errors = 0, header1_errors = 0, header2_errors = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        if (dwt_readdevid() != DWT_DEVICE_ID) id_errors++;

        // offset 0: 1-byte SPI header
        uint32_t value = esp_random();
        dwt_write32bitoffsetreg(PANADR_ID, 0, value);
        if (dwt_read32bitoffsetreg(PANADR_ID, 0) != value) header1_errors++;

        // offset 1: 2-byte SPI header
        value = esp_random();
        dwt_write32bitoffsetreg(DX_TIME_ID, 1, value);
        if (dwt_read32bitoffsetreg(DX_TIME_ID, 1) != value) header2_errors++;
    }
    int errors = id_errors + header1_errors + header2_errors;
    ESP_LOG_LEVEL(errors ? ESP_LOG_ERROR : ESP_LOG_INFO, TAG,
                  "%d kHz, %d cycles: errors DEV_ID %d, 1-byte header %d, 2-byte header %d", hz / 1000, ITERATIONS,
                  id_errors, header1_errors, header2_errors);
}

void spi_test_run(void) {
    dw1000_port_init();
    while (true) {
        // The DW1000 uses its crystal after a reset: SPI must be below 3 MHz
        dw1000_port_set_slowrate();
        dw1000_port_reset();
        check_reg("DEV_ID", dwt_readdevid(), DWT_DEVICE_ID);
        check_reg("SYS_CFG", dwt_read32bitreg(SYS_CFG_ID), SYS_CFG_RESET);
        check_reg("TX_FCTRL", dwt_read32bitreg(TX_FCTRL_ID), TX_FCTRL_RESET);

        if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR) {
            ESP_LOGE(TAG, "dwt_initialise() failed");
        } else {
            // The PLL is locked: the fast rate is permitted
            stress(DW1000_SPI_SLOW_HZ);
            stress(DW1000_SPI_FAST_HZ);
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
