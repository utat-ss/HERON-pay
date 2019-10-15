#ifndef MOTORS_H
#define MOTORS_H

#include <stdbool.h>

#include <avr/io.h>

#include <pex/pex.h>
#include <uart/uart.h>

#include "devices.h"

// Phase/enable mode only
// PEX motor control pins, all on PEX1
// Motor 1, GPIO Bank B
#define MOT1_SLP_N       6
#define MOT1_ADECAY      5
#define MOT1_BDECAY      7
#define MOT1_M1          3
#define MOT1_AENBL       2
#define MOT1_BENBL       1

// Motor 2, GPIO Bank A
#define MOT2_SLP_N       0 //B0
#define MOT2_ADECAY      6
#define MOT2_BDECAY      7
#define MOT2_M1          4
#define MOT2_AENBL       5
#define MOT2_BENBL       3

// Motor phase pins on 64m1
#define MOT1_BPHASE_PIN   PD5
#define MOT1_BPHASE_PORT  PORTD
#define MOT1_BPHASE_DDR   DDRD
#define MOT1_APHASE_PIN   PD7
#define MOT1_APHASE_PORT  PORTD
#define MOT1_APHASE_DDR   DDRD

#define MOT2_BPHASE_PIN   PD6
#define MOT2_BPHASE_PORT  PORTD
#define MOT2_BPHASE_DDR   DDRD
#define MOT2_APHASE_PIN   PB2
#define MOT2_APHASE_PORT  PORTB
#define MOT2_APHASE_DDR   DDRB

// Fault interrupt pins
#define MOT1_FLT_N       4  // B4
#define MOT2_FLT_N       2  // A2

void init_motors(void);
void enable_motors(void);
void disable_motors(void);
void actuate_motors(uint16_t period, uint16_t num_cycles, bool forward);

#endif
