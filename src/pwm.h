#ifndef PWM_H
#define PWM_H

#include <uart/uart.h>
#include <stdint.h>


void OC0A(uint8_t prescaler, uint8_t top);

#endif
