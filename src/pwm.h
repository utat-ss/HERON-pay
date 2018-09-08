#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>

void init_pwm_8bit(uint8_t prescaler, uint8_t top);
void init_pwm_16bit(uint8_t prescaler, uint16_t top, uint16_t duty_cycle);
void stop_timer();
void start_timer();

#endif
