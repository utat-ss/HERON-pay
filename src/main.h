#ifndef MAIN_H
#define MAIN_H

#include <utilities/utilities.h>

// AVR libraries
#include <util/delay.h>

// Standard libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// lib-common libraries
#include <can/can.h>
#include <can/can_ids.h>
#include <can/can_data_protocol.h>
#include <queue/queue.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <adc/adc.h>
#include <pex/pex.h>
#include <conversions/conversions.h>

// PAY libraries
#include "analog_temp.h"
#include "can_callbacks.h"
#include "motors.h"
#include "optical.h"
#include "sensors.h"

void handle_rx_hk(uint8_t* tx_data);
void handle_rx_sci(uint8_t* tx_data);
void handle_rx(void);
void setup_adc(void);
void init_pay(void);

#endif
