#ifndef CAN_COMMANDS_H
#define CAN_COMMANDS_H

#include <stdbool.h>
#include <stdint.h>

#include <adc/adc.h>
#include <can/data_protocol.h>
#include <uptime/uptime.h>
#include <utilities/utilities.h>

#include "boost.h"
#include "can_interface.h"
#include "devices.h"
#include "env_sensors.h"
#include "heaters.h"
#include "motors.h"
#include "optical_spi.h"

extern queue_t rx_msg_queue;
extern queue_t tx_msg_queue;

void handle_rx_msg(void);

#endif
