#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/heaters.h"

//define global variables

#define SETPOINT 37

uint16_t THERM_STATUS;
uint8_t HEATERS_STATUS;
double THERM_READINGS[12];
uint8_t THERM_ERR_CODE[12];

/*
 * ABOUT THERM_ERR_CODE
 *
 * This variable is an indication of why the thermistor failed, or say at which point of the control loop it is eliminated
 * 0 - normal/not eliminated
 * 1 - lower than ultra low limit
 * 2 - higher than ultra high limit
 * 3 - lower than mean (miu) of all 12 thermistors by 3 standard deviation (sigma)
 * 4 - higher than miu by 3 sigma
 * 5, 6, 7 - should not happen
 *
 */

void init_control_loop(void){
    THERM_STATUS = 0x0fff;
    HEATERS_STATUS = 0x00;
    for (uint8_t i = 0; i < 12; i++){
        THERM_READINGS[i] = 0.07 //some specific double, this number doesn't matter anyway
        THERM_ERR_CODE[i] = 0;
    }
}

void acquire_therm_data(void){

}


void heater_control(void){
    acquire_therm_data();
    eliminate_bad_therm();
    average_heaters();
    control_output();
}

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 'e':
            print("Control loop starts\n");
            init_control_loop ();
            heater_control();
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

    init_adc(&adc2);
    print("\nADC2 initialized\n");

    init_spi();
    print("\nSPI initialized\n");

    init_pex(&pex2);
    print("\nPEX2 initialized\n");

    init_heaters();
    print("\nHeaters Initialized\n");

    init_boosts ();
    enable_6V_boost ();
    print("\n6V boost turned on\n");

=======
=======
    print("\nStarting Heaters control test\n\n");

>>>>>>> skeleton of control loop
    set_uart_rx_cb(key_pressed);
    while (1) {}
>>>>>>> heaters manual test done
}
