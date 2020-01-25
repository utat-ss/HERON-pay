#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/optical_spi.h"

// need global variable for spi_in_progress

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


    // SPI transfer parameters
    uint8_t cmd_opcode = CMD_GET_READING;
    uint8_t well_info = _BV(7) | 0x1; // bit7 optical=0, 1=LED;  bit5-0 = 0-31 well number

    // loop through all 32 wells, taking optical and LED readings
    uint8_t well_number = 0;
    uint8_t test_type = 0;

    uint32_t received_data = 0;

    while (1) {
        // print("\nhello world");
        // _delay_ms(500);

        for (well_number = 0; well_number<32; well_number++){
            for (test_type = 0; test_type<2; test_type++){
                // properly encode well_info
                well_info = test_type<<7 | well_number;

                // send command
                send_opt_spi_cmd(cmd_opcode, well_info);

                // wait a while before checking OPTICAL
                _delay_ms(10);

                received_data = check_received_opt_data(NUM_GET_READING);     // expecting 3 return bytes
                print("well_number %d, test_type %d: %X", well_number, test_type, received_data);
            }
        }
    }

    return 0;
}
