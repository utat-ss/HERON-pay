#ifndef OPTICAL_H
#define OPTICAL_H

#include <stdint.h>
#include <stdbool.h>
#include <spi/spi.h>
#include <can/can_data_protocol.h>
#include "can_callbacks.h"

// Uncomment this to disable the use of CAN
//#define DISABLE_CAN

void send_read_sensor_command(uint8_t field_number);

#endif
