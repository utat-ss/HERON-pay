#ifndef OPTICAL_H
#define OPTICAL_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <stdbool.h>
#include <spi/spi.h>
#include <util/delay.h>
#include <can/can_data_protocol.h>
#include "can_callbacks.h"

// Uncomment this to disable the use of CAN
//#define DISABLE_CAN

// SENS_CS on schematic
#define OPTICAL_CS_PIN PB3
#define OPTICAL_CS_PORT PORTB
#define OPTICAL_CS_DDR DDRB

// SENS_RST on schematic
#define OPTICAL_RST_PIN PB4
#define OPTICAL_RST_PORT PORTB
#define OPTICAL_RST_DDR DDRB

void init_optical(void);
void rst_optical(void);
void send_read_sensor_command(uint8_t field_number);

#endif
