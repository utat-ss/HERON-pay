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
        case 'a':
            print("Enabled all heaters\n");
            heater_all_on();
            break;
        case 's':
            print("Disabled all heaters\n");
            heater_all_off();
            break;
        case 'q':
            print("Enabled heater1\n");
            heater_on(1);
            break;
        case 'w':
            print("Enabled heater2\n");
            heater_on(2);
            break;
        case 'e':
            print("Enabled heater3\n");
            heater_on(3);
            break;
        case 'r':
            print("Enabled heater4\n");
            heater_on(4);
            break;
        case 't':
            print("Enabled heater5\n");
            heater_on(5);
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

    init_pex(&pex2);
    print("\nPEX2 initialized\n");

    init_heaters();
    print("Heaters Initialized\n");

    init_boosts ();
    enable_6V_boost ();
    print("6V boost turned on\n");

    init_adc(&adc1);
    init_adc(&adc2);
    print("ADC1 & ADC2 Initialized\n");

    print("\nStarting Heaters test\n\n");

    set_uart_rx_cb(key_pressed);
    while (1) {}
}
