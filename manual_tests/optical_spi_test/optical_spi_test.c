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

    // loop through all 32 wells, taking OD and FL readings
    while (1) {
        for (uint8_t test_type = 0; test_type<2; test_type++){
            for (uint8_t well_number = 0; well_number<32; well_number++){
                // properly encode well_info
                uint8_t field = (test_type << TEST_TYPE_BIT) | well_number;

                print("\n");
                print("Starting field %u (type = %u, well = %u)\n", field,
                    test_type, well_number);

                // send command
                send_opt_spi_cmd(CMD_GET_READING, field);

                // Loop until optical has data ready
                for (uint32_t i = 0; i < 10000; i++) {
                    check_received_opt_data(NUM_GET_READING);     // expecting 3 return bytes

                    // Check if the message is in the TX queue
                    if (queue_size(&tx_msg_queue) > 0) {
                        uint8_t tx_msg[8] = {0x00};
                        dequeue(&tx_msg_queue, tx_msg);

                        uint8_t field_num = tx_msg[1];
                        uint32_t data =
                            ((uint32_t) tx_msg[4] << 24) |
                            ((uint32_t) tx_msg[5] << 16) |
                            ((uint32_t) tx_msg[6] << 8) |
                            ((uint32_t) tx_msg[7] << 0);
                        print("Received field %u: 0x%lx\n", field_num, data);
                        // Break out of for loop to be done with this field
                        break;
                    }

                    _delay_ms(1);
                }

                print("Done field\n");
            }
        }
    }

    return 0;
}
