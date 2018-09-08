#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../../src/pwm.h"

int main(void){

    init_pwm_8bit(4, 255);
    init_pwm_16bit(0, 0xFFFF, 0x0FFF);

}
