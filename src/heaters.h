#ifndef HEATERS_H
#define HEATERS_H

#include <dac/dac.h>

#include "devices.h"

void set_heaters_1_4_raw_setpoint(uint16_t raw_data);
void set_heater_5_raw_setpoint(uint16_t raw_data);

void set_heaters_1_4_setpoint_temp(double temp);
void set_heater_5_setpoint_temp(double temp);

#endif
