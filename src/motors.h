#ifndef MOTORS_H
#define MOTORS_H

#include <stdbool.h>
#include <avr/io.h>
#include <pex/pex.h>
#include <pex/pay.h>
#include <uart/uart.h>

#define F_CPU 8000000UL
#include <util/delay.h>

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

// PEX properties
extern pin_info_t pex_cs;
extern pin_info_t pex_rst;
extern pex_t pex;


void init_motors(void);
void enable_motors(void);
void disable_motors(void);
void actuate_motors(void);

#endif
