#ifndef OPTICAL_SPI_H
#define OPTICAL_SPI_H

#include <stdbool.h>
#include <stdint.h>

#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "can_interface.h"
#include "can_commands.h"

// OPT_CS_N pin on schematic
#define OPT_CS          PB4
#define OPT_CS_PIN      PINB
#define OPT_CS_PORT     PORTB
#define OPT_CS_DDR      DDRB

// OPT_RST_N pin on schematic
#define OPT_RST         PB3
#define OPT_RST_PIN     PINB
#define OPT_RST_PORT    PORTB
#define OPT_RST_DDR     DDRB

// OPT_DATA_RDY pin on schematic
#define OPT_DATA        PC7
#define OPT_DATA_PIN    PINC
#define OPT_DATA_PORT   PORTC
#define OPT_DATA_DDR    DDRC

// test type and field (well) number bits
#define TEST_TYPE_BIT        5          // 0 = optical density, 1 = fluorescence
#define FIELD_NUMBER_BIT     4

// SPI OPCODES, copied from pay-optical/src/spi_comms.h
#define CMD_GET_READING             0x01
#define CMD_GET_POWER               0x02

// unimplemented
#define CMD_SET_LED_BANK            0x04
#define CMD_GET_LED_BANK            0x05
#define CMD_ENTER_SLEEP_MODE        0x06
#define CMD_ENTER_NORMAL_MODE       0x07

// Expected number of bytes to be returned from OPTICAL
#define NUM_GET_READING             3
#define NUM_GET_POWER               3

extern bool spi_in_progress;
extern uint8_t current_well_info;

void init_opt_spi(void);
void rst_opt_spi(void);
uint8_t get_data_pin(void);
void send_opt_spi_cmd(uint8_t cmd_opcode, uint8_t well_info);
void check_received_opt_data(uint8_t num_expected_bytes);
uint8_t opcode_to_num_bytes(uint8_t opcode);

#endif
