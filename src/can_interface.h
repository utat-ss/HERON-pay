#ifndef CAN_CALLBACKS_H
#define CAN_CALLBACKS_H

#include <stdint.h>
#include <can/can.h>
#include <can/ids.h>
#include <can/data_protocol.h>
#include <queue/queue.h>
#include <uart/uart.h>

extern queue_t rx_message_queue;
extern queue_t tx_message_queue;

// CAN MOBs
extern mob_t cmd_rx_mob;
extern mob_t data_tx_mob;

// CAN interrupts
void cmd_rx_callback(const uint8_t* data, uint8_t len);
void data_tx_callback(uint8_t* data, uint8_t* len);

#endif
