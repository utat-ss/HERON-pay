#ifndef OPTICAL_H
#define OPTICAL_H

#include <stdint.h>
#include <stdbool.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <can/data_protocol.h>
#include "can_callbacks.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

// SENS_CS on schematic
#define OPTICAL_CS_PIN PB3
#define OPTICAL_CS_PORT PORTB
#define OPTICAL_CS_DDR DDRB

// SENS_RST on schematic
#define OPTICAL_RST_PIN PB4
#define OPTICAL_RST_PORT PORTB
#define OPTICAL_RST_DDR DDRB

void ssm_spi_init(void);
void opt_spi_init(void);
void opt_spi_rst(void);
void opt_spi_send_read_cmd(uint8_t field_number);

#endif
