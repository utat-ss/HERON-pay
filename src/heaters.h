#ifndef HEATERS_H
#define HEATERS_H

#include <uptime/uptime.h>

#include "devices.h"
=======
#include <avr/eeprom.h>
>>>>>>> devices re-mapping/overhaul

// Heaters all ON/OFF only (no PWM), controlled by PEX2
// All on Bank B
#define HEATER1_EN_N       3
#define HEATER2_EN_N       4
#define HEATER3_EN_N       5
#define HEATER4_EN_N       6
#define HEATER5_EN_N       7

void init_heaters(void);
void heater_all_on(void);
void heater_all_off(void);
void heater_on(uint8_t);
void heater_off(uint8_t);

#endif
