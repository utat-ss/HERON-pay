#include "spi.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

// Assumes bank = 0, which it does on RST
#define IOCON 0x0A  // where config is stored
#define IOCON_DEFAULT 0b00001000 // Only sets hardware addressing

// By default all GPIO pins are input
#define IODIR_BASE 0x00 // where direction is stored. 0 is output.

#define GPIO_BASE 0x12 // where GPIO states are stored.
#define write_control_byte 0b01000000
#define read_control_byte 0b01000001    // Bits 3-1 (MSB first) are A2-A0 for hardware address.

#define CS PB3
#define CS_PORT PORTB
#define CS_DDR DDRB

void port_expander_init(void);
void port_expander_write(uint8_t address, uint8_t register_addr, uint8_t data);
uint8_t port_expander_read(uint8_t address, uint8_t register_addr);
