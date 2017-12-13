#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "sensors.h"
#include "adc.h"
#include <util/delay.h>
#include <uart/uart.h>
#include <uart/log.h>

#include <queue/queue.h>
#include <can/can.h>

Queue* cmd_queue;

void pay_rx_callback(uint8_t* data, uint8_t len);
void tx_callback(uint8_t*, uint8_t*);
void run_cmd(Data cmd);


