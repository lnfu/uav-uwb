#include "spi_unskew.h"

uint8_t spi_unskew(uint8_t prev, const uint8_t *rx, uint8_t *out, size_t len) {
    for (size_t i = 0; i < len; i++) {
        out[i] = (uint8_t)((prev << 7) | (rx[i] >> 1));
        prev = rx[i];
    }
    return prev;
}
