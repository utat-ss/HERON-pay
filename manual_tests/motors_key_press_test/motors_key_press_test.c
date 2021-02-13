/*
Test the motors where you can press 'w' to move the platform up or 's' to move the platform down.
(this is done through UART terminal input)
*/

#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/motors.h"
#include "../../src/boost.h""

#define PERIOD_MS   100
#define NUM_CYCLES  100

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 'w':
            print("Actuating up\n");
            actuate_motors(PERIOD_MS, NUM_CYCLES, true);
            break;
        case 's':
            print("Actuating down\n");
            actuate_motors(PERIOD_MS, NUM_CYCLES, false);
            break;
        case 'o':
            print("Actuating motor1 down\n");
            actuate_motor1((PERIOD_MS/10), (NUM_CYCLES/10), false);
            break;
        case 't':
            print("Actuating motor2 down\n");
            actuate_motor2((PERIOD_MS/10), (NUM_CYCLES/10), false);
            break;
        case 'u':
            print("Actuating motor1 up\n");
            actuate_motor1((PERIOD_MS/10), (NUM_CYCLES/10), true);
            break;
        case 'y':
            print("Actuating motor2 up\n");
            actuate_motor2((PERIOD_MS/10), (NUM_CYCLES/10), true);
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

    init_pex(&pex1);
    init_pex(&pex2);
    print("PEX Initialized\n");

    print("Starting test\n");

    init_boosts();
    enable_10V_boost ();
    print("10V boost enabled\n");

    init_motors();
    print("Motors Initialized\n");

    // Wait for key press interrupts
    // Register callback for key presses
    set_uart_rx_cb(key_pressed);
    while (1) {}
}
