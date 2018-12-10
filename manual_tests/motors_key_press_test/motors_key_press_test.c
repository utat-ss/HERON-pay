/*
Test the motors where you can press 'w' to move the platform up or 's' to move the platform down.
(this is done through UART terminal input)
*/

#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/motors.h"

#define PERIOD_MS   40
#define NUM_CYCLES  15

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    // forward -> up
    // backward -> down
    switch (buf[0]) {
        case 'w':
            print("Actuating up\n");
            actuate_motors(PERIOD_MS, NUM_CYCLES, true);
            break;
        case 's':
            print("Actuating down\n");
            actuate_motors(PERIOD_MS, NUM_CYCLES, false);
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
    set_uart_rx_cb(key_pressed);
    while (1) {}
}
