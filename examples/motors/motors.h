s#ifndef MOTORS_H
#define MOTORS_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>

#include <pex/pex.h>
#include <uart/uart.h>
#include <utilities/utilities.h>

extern pex_t pex;

// PEX CS pin
#define PEX_CS_PIN      PB2
#define PEX_CS_PORT     PORTB
#define PEX_CS_DDR      DDRB

// PEX RST pin
#define PEX_RST_PIN     PB3
#define PEX_RST_PORT    PORTB
#define PEX_RST_DDR     DDRB

// PEX address
#define PEX_ADDR        0b010

//#include "devices.h"

// Reset pins
#define RESET_M1        PB3
#define RESET_M2        PB6
#define MOT_RESET_DDR   DDRB

// Fault interrupt pins
#define FAULT_M1        PB5
#define FAULT_M1_DDR    DDRB
#define FAULT_M1_PORT   PORTB
#define FAULT_M2        PC0
#define FAULT_M2_DDR    DDRC
#define FAULT_M2_PORT   PORTC

// Phase/enable mode
// DRV8843N1 GPIOB
#define PEX_SLP_M1        2
#define PEX_DECAY_M1      3
#define PEX_AI0_M1        7
#define PEX_AI1_M1        6
#define PEX_BI0_M1        5
#define PEX_BI1_M1        4

// DRV8843N2 GPIOA
#define PEX_SLP_M2        2
#define PEX_DECAY_M2      3
#define PEX_AI0_M2        7
#define PEX_AI1_M2        6
#define PEX_BI0_M2        5
#define PEX_BI1_M2        4

// PEX CS pin
#define PEX_CS_PIN      PB2
#define PEX_CS_PORT     PORTB
#define PEX_CS_DDR      DDRB

// DRV8843N1
#define AIN1_M1     PC1
#define AIN2_M1     PC4
#define BIN1_M1     PC5
#define BIN2_M1     PC6
#define PORT_M1     PORTC
#define DDR_M1      DDRC

// DRV8843N2
#define AIN1_M2     PD1
#define AIN2_M2     PD5
#define BIN1_M2     PD6
#define BIN2_M2     PD7
#define PORT_M2     PORTD
#define DDR_M2      DDRD

void init_motors(void);
void enable_motors(void);
void disable_motors(void);
void reset_motors(void);
void actuate_motors(uint16_t period, uint16_t num_cycles, bool forward);

#endif
