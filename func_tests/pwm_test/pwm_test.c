#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../../src/pwm.h"
#include <util/delay.h>

int main(void){
  stop_timer();
  _delay_ms(5000);
  start_timer();
  init_pwm_8bit(4, 255);
  init_pwm_16bit(0, 0xFFFF, 0x0FFF);

}
