#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/heaters.h"

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 'o':
            //"o"n
            print("Enabled all heaters\n");
            heater_all_on();
            break;
        case 'f':
            //o"f"f
            print("Disabled all heaters\n");
            heater_all_off();
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

int main(void){
    init_uart();
    print("\nUART initialized\n");

    init_spi();
    print("\nSPI initialized\n");

    init_pex(&pex1);
    print("\nPEX initialized\n");

    init_heaters();
    print("Heaters Initialized\n");

    print("\nStarting Heaters test\n\n");

    set_uart_rx_cb(key_pressed);
    while (1) {}
}
