#ifndef HEATERS_H
#define HEATERS_H

#include <uptime/uptime.h>

#include "devices.h"
#include <avr/eeprom.h>

// Heaters all ON/OFF only, all on PEX1
//GPIO0 Bank A
#define HEATER_EN_N       0

void init_heaters(void);
void heater_all_on(void);
void heater_all_off(void);

#endif
