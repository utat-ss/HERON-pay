/*
Should uncomment `#define DISABLE_CAN` in optical.h  before running this test
*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include <can/data_protocol.h>
#include "../../src/optical_spi.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    ssm_spi_init();
    print ("SSM SPI Initialized\n");


    opt_spi_init();
    print("PAY-Optical Initialized\n");

    _delay_ms(2000);
    print("Resetting PAY-Optical\n");
    opt_spi_rst();
    print("Done resetting PAY-Optical");
    _delay_ms(2000);

    print("\nStarting test\n\n");

    while (1) {
        for (uint8_t field_num = 0; field_num < CAN_PAY_SCI_FIELD_COUNT; field_num++) {
            print("Sending command #%u\n", field_num);
            opt_spi_send_read_cmd(field_num);
            _delay_ms(3000);
        }
    }
}
