/*
Should uncomment `#define DISABLE_CAN` in optical.h  before running this test
*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include <can/can_data_protocol.h>
#include "../../src/optical.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_optical();
    print("PAY-Optical Initialized\n");

    _delay_ms(2000);
    print("Resetting PAY-Optical\n");
    rst_optical();
    print("Done resetting PAY-Optical");
    _delay_ms(2000);

    print("\nStarting test\n\n");

    while (1) {
        for (uint8_t field_num = 0; field_num < CAN_PAY_SCI_FIELD_COUNT; field_num++) {
            print("Sending command #%u\n", field_num);
            send_read_optical_command(field_num);
            _delay_ms(10000);
        }
    }
}
