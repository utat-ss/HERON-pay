#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <can/data_protocol.h>
#include "can_interface.h"
#include "env_sensors.h"

void handle_rx_msg(void);

#endif
