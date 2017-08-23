#include "spi.h"

// SPI pins
#define CLK PB7
#define MISO PB0
#define MOSI PB1

void init_cs(int pin, PORT ddr) {
    *ddr |= _BV(pin);
}

void set_cs_low(int pin, PORT port) {
    *port &= ~(_BV(pin));
}

void set_cs_high(int pin, PORT port) {
    *port |= _BV(pin);
}

// Before SPI is enables, PPRSPI must be 0
void init_spi(void) {
    // make CLK and MOSI pins output
    DDRB |= _BV(CLK) | _BV(MOSI);
    // enable SPI, set mode to master, set SCK freq to f_io/64
    SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

char send_spi(char cmd) {
    // TODO: the slave device data mode must match the configured data mode;
    // this often works with the defaults, not not always.
    SPDR = cmd;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}
