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

    pex_init(PEX_ADDR_PAY, PEX_BOARD_PAY);
    print("PEX Initialized\n");


    print("Starting test\n");

    init_motors();
    print("Motors Initialized\n");

    while (1) {
        print("Actuating motors for 5 seconds\n");
        actuate_motors();

        print("Waiting for 1 seconds\n");
        _delay_ms(1000);
    }
}
