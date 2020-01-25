#ifndef HEATERS_H
#define HEATERS_H

#include <stdbool.h>
#include <avr/eeprom.h>
#include <uptime/uptime.h>
#include "devices.h"


#define THERMISTOR_COUNT    12
#define HEATER_COUNT        5

// Heaters all ON/OFF only (no PWM), controlled by PEX2
// All on Bank B
#define HEATER1_EN_N       3
#define HEATER2_EN_N       4
#define HEATER3_EN_N       5
#define HEATER4_EN_N       6
#define HEATER5_EN_N       7

//temperature constants, all in degree Celsius
// TODO - unit test to make sure this value converted is what we expect
// TODO - equal to 10C
#define HEATERS_SETPOINT_RAW_DEFAULT 0x800
// TODO have a function to reject invalid setpoint - in CAN
#define THERM_CONV_ULL -40
#define THERM_CONV_UHL 120

#define HEATERS_SETPOINT_EEPROM_ADDR    0x300
// This is for thermistor 0, for each thermistor add 4
#define THERM_ERR_CODE_EEPROM_ADDR_BASE 0x310

/*
 * ABOUT therm_err_codes
 *
 * This variable is an indication of why the thermistor failed, or say at which point of the control loop it is eliminated
 * 0 - normal/not eliminated
 * 1 - lower than ultra low limit (ULL)
 * 2 - higher than ultra high limit (UHL)
 * 3 - lower than mean (miu) of all 12 thermistors by 3 standard deviation (sigma)
 * 4 - higher than miu by 3 sigma
 * 5 - ground manual set to invalid
 * 6 - ground manual set to valid
 * 7 - unused
 */
#define THERM_ERR_CODE_NORMAL           0x00
#define THERM_ERR_CODE_BELOW_ULL        0x01
#define THERM_ERR_CODE_ABOVE_UHL        0x02
#define THERM_ERR_CODE_BELOW_MIU_3SIG   0x03
#define THERM_ERR_CODE_ABOVE_MIU_3SIG   0x04
#define THERM_ERR_CODE_MANUAL_INVALID   0x05
#define THERM_ERR_CODE_MANUAL_VALID     0x06


extern uint16_t therm_readings_raw[];
extern double therm_readings_conv[];
extern uint8_t therm_err_codes[];
extern uint8_t therm_enables[];

extern uint16_t heaters_setpoint_raw;
extern uint8_t heater_enables[];

extern uint32_t heater_ctrl_last_exec_time;


void init_heater_ctrl(void);
void heater_all_on(void);
void heater_all_off(void);
void heater_on(uint8_t);
void heater_off(uint8_t);

//heater control loop stuff
void set_heaters_setpoint_raw(uint16_t setpoint);
uint16_t count_ones(uint8_t* array, uint8_t size);
uint32_t enables_to_uint(uint8_t* enables, uint32_t count);
void acquire_therm_data (void);
void update_therm_statuses (void);
void heater_toggle(double, uint8_t);
void heater_3in_ctrl (void);
void heater_4in_ctrl (void);
void heater_5in_ctrl (void);
void average_heaters (void);
void print_heater_ctrl_status (void);
void run_heater_ctrl (void);
void heater_ctrl_main (void);

#endif
