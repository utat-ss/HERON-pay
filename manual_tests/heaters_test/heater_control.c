#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/boost.h"
#include "../../src/heaters.h"

int main(void){
    init_uart();
    print("\nUART initialized\n");

    init_adc(&adc2);
    print("\nADC2 initialized\n");

    init_spi();
    print("\nSPI initialized\n");

    init_pex(&pex2);
    print("\nPEX2 initialized\n");

    init_heater_ctrl();
    print("\nHeaters Initialized\n");

    init_boosts ();
    enable_6V_boost ();
    print("\n6V boost turned on\n");

    print("\nStarting Heaters control test\n\n");

    while (1) {
        run_heater_control();

        // current delay, every 20 seconds
        _delay_ms(20000);
    }
}
