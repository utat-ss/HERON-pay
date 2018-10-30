#ifndef CAN_CALLBACKS_H
#define CAN_CALLBACKS_H

#include <stdint.h>
#include <can/can.h>
#include <can/ids.h>
#include <can/data_protocol.h>
#include <queue/queue.h>
#include <uart/uart.h>

extern queue_t can_rx_msgs;
extern queue_t can_tx_msgs;

// CAN MOBs
extern mob_t cmd_rx_mob;
extern mob_t data_tx_mob;

#endif
