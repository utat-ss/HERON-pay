 /*
For pay-ssm v4, all heaters are controlled with software via PEX pins

For Ganymede, all heater control pins are tied to PEX1, GPIOB0

For now no PWM control, just turn it all ON or all OFF all the time.

To-do: PWM control (probably 1K frequency)

Author: Lorna Lan
 */

#include "heaters.h"
=======
>>>>>>> devices re-mapping/overhaul
=======

void init_heaters(void){
  set_pex_pin_dir(&pex2, PEX_B, HEATER1_EN_N, OUTPUT);
  set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 1);

  set_pex_pin_dir(&pex2, PEX_B, HEATER2_EN_N, OUTPUT);
  set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 1);

  set_pex_pin_dir(&pex2, PEX_B, HEATER3_EN_N, OUTPUT);
  set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 1);

  set_pex_pin_dir(&pex2, PEX_B, HEATER4_EN_N, OUTPUT);
  set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 1);

  set_pex_pin_dir(&pex2, PEX_B, HEATER5_EN_N, OUTPUT);
  set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 1);
}

void heater_all_on(void) {
    set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 0);
}

void heater_all_off(void) {
    set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 1);
}

void heater_on(uint8_t heater_num){
    switch(heater_num){
    case 1:
        set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 0);
        break;
    case 2:
        set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 0);
        break;
    case 3:
        set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 0);
        break;
    case 4:
        set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 0);
        break;
    case 5:
        set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 0);
        break;
    default:
        return;
        break;
    }
}

void heater_on(uint8_t heater_num){
    switch(heater_num){
    case 1:
        set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 1);
        break;
    case 2:
        set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 1);
        break;
    case 3:
        set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 1);
        break;
    case 4:
        set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 1);
        break;
    case 5:
        set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 1);
        break;
    default:
        return;
        break;
    }
}
>>>>>>> Heaters control src
