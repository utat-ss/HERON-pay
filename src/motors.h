#ifndef MOTORS_H
#define MOTORS_H

#include <avr/io.h>
#include <pex/pex.h>

// For both GPIOA (GPA) and GPIOB (GPB)
#define PEX_M1      7
#define PEX_M0      6
#define PEX_EN      5
#define PEX_SLP     4
#define PEX_DIR     3
#define PEX_DECAY   2

// Motor step input
#define STEP_PIN    PC1
#define STEP_PORT   PORTC
#define STEP_DDR    DDRC

void init_motors(void);
void actuate_motors(void);

#endif
