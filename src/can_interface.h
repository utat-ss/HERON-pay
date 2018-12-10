#ifndef CAN_INTERFACE_H
#define CAN_INTERFACE_H

#include <stdint.h>

#include <can/can.h>
#include <can/data_protocol.h>
#include <can/ids.h>
#include <queue/queue.h>
#include <uart/uart.h>

#include "can_commands.h"

extern mob_t cmd_rx_mob;
extern mob_t data_tx_mob;

#endif
