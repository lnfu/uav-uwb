#pragma once

#include <stddef.h>
#include <stdint.h>

// Corrects DW1000 SPI read data on the AmazingUWB board. The ESP32-S3 samples
// MISO half a clock cycle late, so each data bit arrives 1 bit position early:
// the first bit of a byte is in the last bit position of the byte before it.
//
// prev is the raw byte before rx[0]. For the first data byte, this is the byte
// that the ESP32-S3 received during the last header byte. The function returns
// the last raw byte, which is prev for the next part of the same transfer.
uint8_t spi_unskew(uint8_t prev, const uint8_t *rx, uint8_t *out, size_t len);
