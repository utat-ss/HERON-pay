#ifndef OPTICAL_SPI_H
#define OPTICAL_SPI_H

#include <stdbool.h>
#include <stdint.h>

#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "can_interface.h"

// OPT_CS_N pin on schematic
#define OPT_CS_PIN      PB4
#define OPT_CS_PORT     PORTB
#define OPT_CS_DDR      DDRB

// OPT_RST_N pin on schematic
#define OPT_RST_PIN     PB3
#define OPT_RST_PORT    PORTB
#define OPT_RST_DDR     DDRB

// OPT_DATA_RDY pin on schematic
#define OPT_RST_PIN     PC7
#define OPT_RST_PORT    PORTC
#define OPT_RST_DDR     DDRC

void init_opt_spi(void);
void rst_opt_spi(void);
uint32_t read_opt_spi(uint8_t field_num);
void send_opt_spi_cmd(uint8_t field_num);

#endif
