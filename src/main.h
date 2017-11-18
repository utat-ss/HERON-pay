#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "sensors.h"
#include "adc.h"
#include <util/delay.h>
#include <uart/uart.h>
#include <uart/log.h>

// #include <queue/queue.h>
#include <can/can.h>


int main (void);
void sensor_led_sequence(void);
void adc_sequence(void);
