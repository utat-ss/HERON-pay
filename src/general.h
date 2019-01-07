#ifndef GENERAL_H
#define GENERAL_H

// Standard libraries
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// lib-common libraries
#include <adc/adc.h>
#include <adc/pay.h>
#include <can/can.h>
#include <can/data_protocol.h>
#include <can/ids.h>
#include <conversions/conversions.h>
#include <pex/pay.h>
#include <pex/pex.h>
#include <queue/queue.h>
#include <spi/spi.h>
#include <uart/uart.h>

// PAY libraries
#include "can_commands.h"
#include "can_interface.h"
#include "devices.h"
#include "env_sensors.h"
#include "motors.h"
#include "optical_spi.h"

void init_pay(void);
void process_next_rx_msg(void);
void send_next_tx_msg(void);

#endif
