#pragma once

#define DW1000_PIN_IRQ 16
#define DW1000_PIN_RST 15
#define DW1000_PIN_CLK 12
#define DW1000_PIN_MISO 13
#define DW1000_PIN_MOSI 11
#define DW1000_PIN_CS 10

// Below 3 MHz until dwt_initialise() locks the PLL. The 1-bit skew holds
// only up to 10 MHz on this board, so the fast rate keeps a step of margin.
#define DW1000_SPI_SLOW_HZ 2000000
#define DW1000_SPI_FAST_HZ 8000000

void dw1000_port_init(void);
void dw1000_port_reset(void);
void dw1000_port_set_rate(int hz);
void dw1000_port_set_slowrate(void);
void dw1000_port_set_fastrate(void);
