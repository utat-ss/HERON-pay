#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <queue/queue.h>

#include "sensors.h"
#include "adc.h"
#include "pex.h"
#include "freq_measure.h"

int main (void);
void sensor_led_sequence(void);
void adc_test_sequence(void);

void poll_adpd(void);
void poll_sfh(void);
void poll_pht(void);
void setup_adc(void);
void poll_int(void);

Queue* cmd_queue;

void pay_rx_callback(uint8_t* data, uint8_t len);
void tx_callback(uint8_t*, uint8_t*);
void run_cmd(Data cmd);
