/*
Test the motors where you can press 'w' to move the platform up or 's' to move the platform down.
(this is done through UART terminal input)
*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include <pex/pex.h>
#include "../../src/motors.h"


uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    // forward -> up
    // backward -> down
    switch (buf[0]) {
        case 'w':
            print("Actuating up\n");
            actuate_motors(100, 10, true);
            break;
        case 's':
            print("Actuating down\n");
            actuate_motors(100, 10, false);
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

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

    // Wait for key press interrupts
    // Register callback for key presses
    register_callback(key_pressed);
    while (1) {}
}
