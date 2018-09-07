#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>

void func_OC0A(uint8_t prescaler, uint8_t top);
void func_OC1B(uint8_t prescaler, uint16_t top);

#endif
