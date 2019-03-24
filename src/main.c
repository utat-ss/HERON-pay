/*
Main functions to be implemented on the PAY 32M1

AUTHORS: Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
*/

#include "general.h"

int main(void) {
    WDT_OFF();
    WDT_ENABLE_SYS_RESET(WDTO_8S);

    init_pay();

    // Main loop
    while (1) {
        WDT_ENABLE_SYS_RESET(WDTO_8S);
        send_next_tx_msg();
        process_next_rx_msg();
    }

    return 0;
}
