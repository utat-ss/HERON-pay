#ifndef CAN_COMMANDS_H
#define CAN_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>

#include <adc/adc.h>
#include <can/data_protocol.h>
#include <utilities/utilities.h>

#include "can_interface.h"
#include "devices.h"
#include "env_sensors.h"
#include "optical_spi.h"

extern queue_t rx_msg_queue;
extern queue_t tx_msg_queue;

extern bool sim_local_actions;

void handle_rx_msg(void);

#endif
