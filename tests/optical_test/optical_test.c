/*
Should uncomment `#define DISABLE_CAN` in optical.h  before running this test
*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include "../../src/optical.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    print("\nStarting test\n\n");

    for (uint8_t field_num = 0; field_num < 33; field_num++) {
        send_read_sensor_command(field_num);
        _delay_ms(10000);
    }
}
