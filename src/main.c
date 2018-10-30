/*
Main functions to be implemented on the PAY 32M1

AUTHORS: Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
*/

#include "pay.h"

int main(void) {
    init_pay();

    // Main loop
    while (1) {
        handle_next_rx_msg();
        send_next_tx_msg();
    }
}
