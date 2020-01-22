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
    uint8_t well_data = 0b10000000; // bit7 optical=0, 1=LED;  bit5-0 = 0-31 well number

    while (1) {
        // print("\nhello world");
        // _delay_ms(500);

        uint32_t data = get_measurement(well_data);
        print("Data in well number %d: %U\n", well_data & 0x1F, data);
    }

    return 0;
}
