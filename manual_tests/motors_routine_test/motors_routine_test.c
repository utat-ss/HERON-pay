#include <pex/pex.h>
#include <uart/uart.h>
#include <spi/spi.h>
#include <uptime/uptime.h>

#include "../../src/motors.h"
#include "../../src/boost.h"

#define PERIOD_MS   100
#define NUM_CYCLES  1

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 'e':
            print("routine starts (draft)\n");

            enable_10V_boost ();
            disable_6V_boost ();

            uint8_t lim_swt1_status, lim_swt2_status;

            // step only one motor at a time
            // always check if the limit switches are pressed
            // if so then stop actuation
            while(1){
                lim_swt1_status = get_pex_pin(&pex2, PEX_A, LIM_SWT1_PRESSED);
                lim_swt2_status = get_pex_pin(&pex2, PEX_A, LIM_SWT2_PRESSED);
                if(!(lim_swt1_status && lim_swt2_status)){
                    actuate_motor1(PERIOD_MS, NUM_CYCLES, true);
                    actuate_motor2(PERIOD_MS, NUM_CYCLES, true);
                    print("motors actuated\n");
                } else {
                    print("both limit switch pressed\n");
                }
            }
            break;
        case 's':
            print("routine from src\n");
            motors_routine();
            //print("routine status:%d\n",motor_routine_status);
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
    print("SPI initialized\n");

    init_pex(&pex1);
    init_pex(&pex2);
    print("PEXes Initialized\n");

    init_boosts();
    print("Boosts Initialized\n");

    print("Starting test\n");

    init_motors();
    print("Motors Initialized\n");

    set_uart_rx_cb(key_pressed);
    while (1) {}
}
