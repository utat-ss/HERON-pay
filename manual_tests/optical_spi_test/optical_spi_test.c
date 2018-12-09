#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/optical_spi.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    opt_spi_enable_can = false;
    opt_spi_init();
    print("Optical SPI Initialized\n");

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
            _delay_ms(1000);
            print("\n");
        }
    }
}
