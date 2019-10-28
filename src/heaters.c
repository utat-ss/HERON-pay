 /*
For pay-ssm v4, all heaters are controlled with software via PEX pins

For Ganymede, all heater control pins are tied to PEX1, GPIOB0

For now no PWM control, just turn it all ON or all OFF all the time.

To-do: PWM control (probably 1K frequency)

Author: Lorna Lan
 */

#include "heaters.h"

void init_heaters(void){
  set_pex_pin_dir(&pex1, PEX_B, HEATER_EN_N, OUTPUT);
  set_pex_pin(&pex1, PEX_B, HEATER_EN_N, 1);
}

void heater_all_on(void) {
    set_pex_pin(&pex1, PEX_B, HEATER_EN_N, 0);
}

void heater_all_off(void) {
    set_pex_pin(&pex1, PEX_B, HEATER_EN_N, 1);
}
