#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/optical_spi.h"

int main(void){
    init_uart();
    print("UART Initialized\n");

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

    uint8_t field_num = 0;
    uint32_t data = 0;

    while (1) {
        for (uint8_t bank = 0; bank < 4; bank ++){
            for (uint8_t channel = 0; channel < 8; channel++){
                // clear field_num
                field_num = 0;

                // bit 5 is set to 1 (optical density)
                field_num = (0b11 << 6) | (0x1<<5) | ((bank & 0b11) << 3) | (channel & 0b111);

                // bit 5 is set to 0 (fluorescence)
                // field_num = (0b11 << 6) |((bank & 0b11) << 3) | (channel & 0b111);
                //  field_num = field_num & ~_BV(5);

                data = read_opt_spi(field_num);
                print("A%d_%d = 0x%06lx--> %-4.2f%\n", bank+1, channel+1, data, (float)data/(float)0xffffff * 100);
                print("\n");

                _delay_ms(1000);
            }
            print("\n");
        }

        /*
        for (uint8_t field_num = 0; field_num < CAN_PAY_OPT_FIELD_COUNT; field_num++) {
            uint32_t data = read_opt_spi(field_num);
            print("Field #%u = 0x%06lx\n", field_num, data);
        }
        */

    }
}
