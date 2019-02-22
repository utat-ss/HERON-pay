#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/optical_spi.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_opt_spi();
    print("Optical SPI Initialized\n");

    _delay_ms(100);
    print("Resetting PAY-Optical\n");
    rst_opt_spi();
    print("Done resetting PAY-Optical");
    _delay_ms(100);

    print("\nStarting test\n\n");

    while (1) {
        for (uint8_t field_num = 0; field_num < CAN_PAY_OPT_FIELD_COUNT; field_num++) {
            uint32_t data = read_opt_spi(field_num);
            print("Field #%u = 0x%06lx\n", field_num, data);
        }

        _delay_ms(5000);
        print("\n");
    }
}
