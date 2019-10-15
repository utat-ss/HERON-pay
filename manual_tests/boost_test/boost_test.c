#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/boost.h"

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 't':
            //"t"en
            print("Enabled 10V boost\n");
            enable_10V_boost();
            break;
        case 's':
            //"s"ix
            print("Enabled 6V boost\n");
            enable_6V_boost();
            break;
        case 'q':
            //arbitrary
            print("Disabling 10V boost\n");
            disable_10V_boost();
            break;
        case 'w':
            //arbitrary
            print("Disabling 6V boost\n");
            disable_6V_boost();
            break;
        case 'e':
            //"e"mergency stop if anything goes wrong
            print("Disabling both boost\n");
            disable_10V_boost();
            disable_6V_boost();
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_boosts();
    print("Boost PEX initialized\n");

    print("\nStarting Boost test\n\n");

    set_uart_rx_cb(key_pressed);
    while (1) {}
}
