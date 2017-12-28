#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "sensors.h"
#include "adc.h"
#include <util/delay.h>
#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <queue/queue.h>
#include <spi/spi.h>

#include "sensors.h"
#include "adc.h"
#include "pex.h"
#include "freq_measure.h"

int main (void);
void sensor_led_sequence(void);
void adc_test_sequence(void);

#include <queue/queue.h>
#include <can/can.h>

Queue* cmd_queue;

void pay_rx_callback(uint8_t* data, uint8_t len);
void tx_callback(uint8_t*, uint8_t*);
void run_cmd(Data cmd);

#define THERMISTOR_BASE 0b10001100
#define PORT    PORTC
#define DDR     DDRC
#define PIN     PC6

void thermistor_testing(void);
