#ifndef MOTORS_H
#define MOTORS_H

#include <stdbool.h>

#include <avr/io.h>

#include <pex/pay.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "devices.h"

// Indexer mode
// For both GPIOA (GPA) and GPIOB (GPB)
// #define PEX_M1      7
// #define PEX_M0      6
// #define PEX_EN      5
// #define PEX_SLP     4
// #define PEX_DIR     3
// #define PEX_DECAY   2
// #define STEP_PIN    PC1
// #define STEP_PORT   PORTC
// #define STEP_DDR    DDRC

// Phase/enable mode
#define MOT_SLP         4
#define MOT_ADECAY      2
#define MOT_M1          7
#define MOT_APHASE      6
#define MOT_BPHASE      3
#define MOT_AENBL       5
#define MOT_BENBL_PIN   PC1
#define MOT_BENBL_PORT  PORTC
#define MOT_BENBL_DDR   DDRC

// Fault interrupt pins
#define MOT_FLT_A       0   // A0
#define MOT_FLT_B       1   // A1

void init_motors(void);
void enable_motors(void);
void disable_motors(void);
void actuate_motors(uint16_t period, uint16_t num_cycles, bool forward);

#endif
