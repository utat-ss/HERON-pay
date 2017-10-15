#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include <spi/spi.h>

// Assumes bank = 0, which it does on RST
#define IOCON 0x0A  // where config is stored
#define IOCON_DEFAULT 0b00001000 // Only sets hardware addressing

// By default all GPIO pins are input
#define IODIR_BASE 0x00 // where direction is stored. 0 is output.
#define GPIO_BASE 0x12 // where GPIO states are stored.
#define write_control_byte 0b01000000
#define read_control_byte 0b01000001    // Bits 3-1 (MSB first) are A2-A0 for hardware address.

#define CS PB5  // Only true for PAY-SSM v2
#define CS_PORT PORTB
#define CS_DDR DDRB

#define RST PB6
#define RST_PORT PORTB
#define RST_DDR DDRB

// PEX ADDRESSES
#define SSM 0
#define LED_PCB 1
#define SENSOR_PCB 2

//GPIOB
#define PRES_CS 0
#define TEMP_CS 1
#define TEMP_SHUTDOWN 2
#define HUMID_CS 3
#define CS_1 4
#define CS_2 5
#define PEX_LED 6


void port_expander_init(void);
void port_expander_write(uint8_t address, uint8_t register_addr, uint8_t data);
uint8_t port_expander_read(uint8_t address, uint8_t register_addr);
void set_gpio_b(uint8_t address, uint8_t pin);
void set_gpio_a(uint8_t address, uint8_t pin);
void clear_gpio_a(uint8_t address, uint8_t pin);
void clear_gpio_b(uint8_t address, uint8_t pin);
void set_dir_a(uint8_t address, uint8_t pin, uint8_t state);
void set_dir_b(uint8_t address, uint8_t pin, uint8_t state);
void reset_pex(void);
