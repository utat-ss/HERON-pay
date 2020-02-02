/*
Main functions to be implemented on the PAY 64M1

AUTHORS: Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown, Lorna Lan
*/

#include "general.h"

int main(void) {
    WDT_OFF();
    WDT_ENABLE_SYS_RESET(WDTO_8S);

    init_pay();
    init_hb(HB_PAY);

    print("\n\n");
    print("PAY main init\n");

    // Run once at the beginning
    run_heater_ctrl();

    // Main loop
    while (1) {
        WDT_ENABLE_SYS_RESET(WDTO_8S);
        run_hb();
        send_next_tx_msg();
        process_next_rx_msg();
        heater_ctrl_main ();

        // some call to check if opt_DATA_RDYn is pulled low
    }

    return 0;
}
