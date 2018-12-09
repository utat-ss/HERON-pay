#ifndef CAN_COMMANDS_H
#define CAN_COMMANDS_H

#include <stdint.h>

#include <can/data_protocol.h>
#include <utilities/utilities.h>

#include "can_interface.h"
#include "env_sensors.h"

extern queue_t rx_msg_queue;
extern queue_t tx_msg_queue;

void handle_rx_msg(void);

#endif
