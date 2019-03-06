#ifndef HEATERS_H
#define HEATERS_H

#include <dac/dac.h>

#include "devices.h"

void set_heaters_1_to_4_raw_setpoint(uint16_t raw_data);
void set_heater_5_raw_setpoint(uint16_t raw_data);

void set_heaters_1_to_4_temp_setpoint(double temp);
void set_heater_5_temp_setpoint(double temp);

#endif
