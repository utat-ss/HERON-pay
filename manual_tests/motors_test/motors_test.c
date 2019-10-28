#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/motors.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_pex(&pex);
    print("PEX Initialized\n");

    print("Starting test\n");

    init_motors();
    print("Motors Initialized\n");

    while (1) {
        print("Actuating: 100ms, 100 times, forward\n");
        actuate_motors(100, 100, true);
        print("Done actuating\n");

        print("Waiting 2 seconds\n");
        _delay_ms(2000);

        print("Actuating: 16ms, 75 times, backward\n");
        actuate_motors(16, 75, false);
        print("Done actuating\n");

        print("Waiting 2 seconds\n");
        _delay_ms(2000);
    }
}
