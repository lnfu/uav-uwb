#include <stdlib.h>
#include <string.h>

#include "spi_unskew.h"
#include "unity.h"
#include "unity_test_runner.h"

// Makes the raw bytes that the ESP32-S3 receives for the given data. Returns
// the raw byte of the last header byte, which holds the first data bit.
static uint8_t skew(const uint8_t *data, uint8_t *raw, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t next_msb = (i + 1 < len) ? (data[i + 1] >> 7) : 0;
        raw[i] = (uint8_t)((data[i] << 1) | next_msb);
    }
    return len ? (data[0] >> 7) : 0;
}

TEST_CASE("DEV_ID as measured on the board", "[spi_unskew]") {
    // background.md: the bytes 30 01 CA DE arrived as 60 03 95 BC
    const uint8_t raw[] = {0x60, 0x03, 0x95, 0xBC};
    const uint8_t expected[] = {0x30, 0x01, 0xCA, 0xDE};
    uint8_t out[sizeof(raw)];
    spi_unskew(0x00, raw, out, sizeof(raw));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, out, sizeof(expected));
}

TEST_CASE("first bit comes from the last bit of prev", "[spi_unskew]") {
    const uint8_t raw[] = {0x00};
    uint8_t out[1];
    spi_unskew(0x01, raw, out, 1);
    TEST_ASSERT_EQUAL_HEX8(0x80, out[0]);
    spi_unskew(0xFE, raw, out, 1);
    TEST_ASSERT_EQUAL_HEX8(0x00, out[0]);
}

TEST_CASE("returns the last raw byte", "[spi_unskew]") {
    const uint8_t raw[] = {0x12, 0x34, 0x57};
    uint8_t out[sizeof(raw)];
    TEST_ASSERT_EQUAL_HEX8(0x57, spi_unskew(0x00, raw, out, sizeof(raw)));
}

TEST_CASE("length 0 writes nothing and returns prev", "[spi_unskew]") {
    uint8_t out[1] = {0xA5};
    TEST_ASSERT_EQUAL_HEX8(0x01, spi_unskew(0x01, NULL, out, 0));
    TEST_ASSERT_EQUAL_HEX8(0xA5, out[0]);
}

TEST_CASE("random data, in one part and in two parts", "[spi_unskew]") {
    enum { LEN = 64 };
    uint8_t data[LEN], raw[LEN], out[LEN];
    srand(1);
    for (int round = 0; round < 1000; round++) {
        for (size_t i = 0; i < LEN; i++) data[i] = (uint8_t)rand();
        uint8_t header_raw = skew(data, raw, LEN);

        spi_unskew(header_raw, raw, out, LEN);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(data, out, LEN);

        // A transfer in two parts must give the same result
        size_t split = (size_t)rand() % (LEN + 1);
        memset(out, 0, LEN);
        uint8_t prev = spi_unskew(header_raw, raw, out, split);
        spi_unskew(prev, raw + split, out + split, LEN - split);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(data, out, LEN);
    }
}
