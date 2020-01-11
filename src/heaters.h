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

// TODO make setpoint a variable instead of a constant since it can't be controlled
// TODO have a function to reject invalid setpoint
#define SETPOINT 33
#define ULL -40
#define UHL 120

void init_heaters(void);
void heater_all_on(void);
void heater_all_off(void);
void heater_on(uint8_t);
void heater_off(uint8_t);

//heater control loop stuff
uint16_t count_ones(uint16_t);
void init_control_loop (void);
void acquire_therm_data (void);
void eliminate_bad_therm (void);
void heater_toggle(double, uint8_t);
void heater_3in_ctrl (void);
void heater_4in_ctrl (void);
void heater_5in_ctrl (void);
void average_heaters (void);
void heater_ctrl_status (void);
void heater_control (void);
void heater_ctrl_main (void);

#endif
