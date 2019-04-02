#ifndef HEATERS_H
#define HEATERS_H

#include <dac/dac.h>

#include "devices.h"

#define EEPROM_DEF_DWORD 0xFFFFFFFF

//EEPROM address for storing raw data
#define HEATER_1_TO_4_RAW_SETPOINT_ADDR ((uint32_t*) 0x80)
#define HEATER_5_RAW_SETPOINT_ADDR ((uint32_t*) 0x84)

void set_heaters_1_to_4_raw_setpoint(uint16_t raw_data);
void set_heater_5_raw_setpoint(uint16_t raw_data);

void set_heaters_1_to_4_temp_setpoint(double temp);
void set_heater_5_temp_setpoint(double temp);

void init_heaters();

#endif
