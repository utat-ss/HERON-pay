#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include "../../src/motors.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    pex_init();
    print("PEX Initialized\n");

    init_motors();
    print("Motors Initialized\n");

    print("\nStarting test\n\n");

    while (1) {
        print("Actuating motors...\n");
        actuate_motors();

        print("Waiting...\n");
        _delay_ms(10000);
    }
}
