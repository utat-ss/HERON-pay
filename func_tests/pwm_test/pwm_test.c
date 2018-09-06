#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../../src/pwm.h"
#include <uart/uart.h>
#include <util/delay.h>

int main(void){
    init_uart();

    OC0A(0, 200);

}
