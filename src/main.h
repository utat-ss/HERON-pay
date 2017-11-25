#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
// #include <queue/queue.h>

#include "sensors.h"
#include "adc.h"
#include "pex.h"

int main (void);
void sensor_led_sequence(void);
void adc_test_sequence(void);
