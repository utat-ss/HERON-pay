#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include <pex/pex.h>
#include "../../src/motors.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_pex(&pex);
    print("PEX Initialized\n");


    print("Starting test\n");

    init_motors();
    print("Motors Initialized\n");

    while (1) {
        print("Actuating: 100ms, 50 times, forward\n");
        actuate_motors(100, 50, true);
        print("Done actuating\n");

        print("Waiting 2 seconds\n");
        _delay_ms(2000);

        print("Actuating: 16ms, 100 times, backward\n");
        actuate_motors(16, 100, false);
        print("Done actuating\n");

        print("Waiting 2 seconds\n");
        _delay_ms(2000);
    }
}
