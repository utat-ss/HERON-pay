#include "spi.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

////port expander stuff
#define write_control_byte 0b01000000
#define read_control_byte 0b01000001    // Bits 3-1 (MSB first) are A2-A0 for hardware address. Currently disabled.
#define GPIOA_config 0b00000000        // GPIOA 0-5 are LEDs
#define GPIOB_config 0b00000000        // GPIOB 2-7 are LEDs, bit 0 is ADC CS
#define IODIR_BASE 0x00
#define GPIO_BASE 0x12
#define IOCON 0x0A

// SPI pins
#define CLK PB7
#define MISO PB0
#define MOSI PB1

#define CS PB2
#define CS_PORT PORTB
#define CS_DDR DDRB


void port_expander_init(void);
void port_expander_write(uint8_t register_addr, uint8_t data);
char port_expander_read(uint8_t register_addr);
void blinkLED_PEX();
