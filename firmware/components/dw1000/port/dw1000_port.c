#include "dw1000_port.h"

#include <stdbool.h>
#include <string.h>
#include <sys/param.h>

#include "deca_device_api.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spi_unskew.h"

// Mode 3; reads arrive one bit early because the S3 samples MISO half a
// cycle late. readfromspi() corrects reads with spi_unskew(); writes need no fix.
#define DW_SPI_MODE 3
#define DW_SPI_HOST SPI2_HOST

// Longer transfers are split into chunks with CS held low.
#ifndef DW1000_SPI_CHUNK
#define DW1000_SPI_CHUNK 256
#endif

static spi_device_handle_t s_dev;
static DMA_ATTR uint8_t s_tx[DW1000_SPI_CHUNK];
static DMA_ATTR uint8_t s_rx[DW1000_SPI_CHUNK];

void dw1000_port_init(void) {
    gpio_set_direction(DW1000_PIN_IRQ, GPIO_MODE_INPUT);
    spi_bus_config_t bus = {
        .mosi_io_num = DW1000_PIN_MOSI,
        .miso_io_num = DW1000_PIN_MISO,
        .sclk_io_num = DW1000_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = DW1000_SPI_CHUNK,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(DW_SPI_HOST, &bus, SPI_DMA_CH_AUTO));
    dw1000_port_set_slowrate();
}

void dw1000_port_reset(void) {
    // RSTn is open-drain: drive low to reset, then release
    gpio_set_direction(DW1000_PIN_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(DW1000_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    gpio_set_direction(DW1000_PIN_RST, GPIO_MODE_INPUT);
    vTaskDelay(pdMS_TO_TICKS(5));
}

void dw1000_port_set_rate(int hz) {
    if (s_dev) {
        ESP_ERROR_CHECK(spi_bus_remove_device(s_dev));
    }
    spi_device_interface_config_t cfg = {
        .mode = DW_SPI_MODE,
        .clock_speed_hz = hz,
        .spics_io_num = DW1000_PIN_CS,
        .queue_size = 1,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(DW_SPI_HOST, &cfg, &s_dev));
}

void dw1000_port_set_slowrate(void) { dw1000_port_set_rate(DW1000_SPI_SLOW_HZ); }

void dw1000_port_set_fastrate(void) { dw1000_port_set_rate(DW1000_SPI_FAST_HZ); }

static void xfer_chunk(size_t len, bool last) {
    spi_transaction_t t = {
        .flags = last ? 0 : SPI_TRANS_CS_KEEP_ACTIVE,
        .length = len * 8,
        .tx_buffer = s_tx,
        .rx_buffer = s_rx,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(s_dev, &t));
}

int writetospi(uint16 headerLength, const uint8 *headerBuffer, uint32 bodylength, const uint8 *bodyBuffer) {
    ESP_ERROR_CHECK(spi_device_acquire_bus(s_dev, portMAX_DELAY));
    memcpy(s_tx, headerBuffer, headerLength);
    xfer_chunk(headerLength, bodylength == 0);
    while (bodylength > 0) {
        size_t n = MIN(bodylength, DW1000_SPI_CHUNK);
        memcpy(s_tx, bodyBuffer, n);
        bodyBuffer += n;
        bodylength -= n;
        xfer_chunk(n, bodylength == 0);
    }
    spi_device_release_bus(s_dev);
    return 0;
}

int readfromspi(uint16 headerLength, const uint8 *headerBuffer, uint32 readlength, uint8 *readBuffer) {
    ESP_ERROR_CHECK(spi_device_acquire_bus(s_dev, portMAX_DELAY));
    memcpy(s_tx, headerBuffer, headerLength);
    xfer_chunk(headerLength, readlength == 0);
    // the MSB of each data byte was latched in the previous byte's LSB slot
    uint8_t prev = s_rx[headerLength - 1];
    while (readlength > 0) {
        size_t n = MIN(readlength, DW1000_SPI_CHUNK);
        memset(s_tx, 0, n);  // MOSI low while reading, as dwt_spicswakeup() expects
        readlength -= n;
        xfer_chunk(n, readlength == 0);
        prev = spi_unskew(prev, s_rx, readBuffer, n);
        readBuffer += n;
    }
    spi_device_release_bus(s_dev);
    return 0;
}

// No DW1000 IRQ handler yet (polled driver), so nothing to mask.
decaIrqStatus_t decamutexon(void) { return 0; }

void decamutexoff(decaIrqStatus_t s) { (void)s; }

void deca_sleep(unsigned int time_ms) { vTaskDelay(pdMS_TO_TICKS(time_ms)); }
