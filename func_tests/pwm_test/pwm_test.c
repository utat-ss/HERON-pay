#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../../src/pwm.h"

int main(void){

    func_OC0A(4, 255);
    func_OC1B(0, 1);

}
