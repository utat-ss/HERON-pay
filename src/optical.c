#include "optical.h"

uint8_t spi_rx_data_field_number = 0;
uint32_t spi_rx_data = 0;
bool spi_rx_data_in_progress = false;
uint8_t spi_rx_data_num_bytes_received = 0; // number of bytes already received from PAY-Optical


// TODO - INT1 or PCINT1?
// PB2/INT1 interrupt
ISR(INT1_vect) {
    print("Interrupt - INT1 (interrupt 1, PB2, from PAY-Optical)!\n");

    // TODO - is it possible to send SPI inside an interrupt?

    if (spi_rx_data_in_progress) {
        spi_rx_data = spi_rx_data << 8;
        spi_rx_data = spi_rx_data | send_spi(0x00);
        spi_rx_data_num_bytes_received++;

        if (spi_rx_data_num_bytes_received >= 3) {
            spi_rx_data_in_progress = false;
            spi_rx_data_num_bytes_received = 0;

            uint8_t tx_data[8];
            tx_data[0] = 0; // TODO
            tx_data[1] = CAN_PAY_SCI;
            tx_data[2] = spi_rx_data_field_number;

            tx_data[3] = (spi_rx_data >> 16) & 0xFF;
            tx_data[4] = (spi_rx_data >> 8) & 0xFF;
            tx_data[5] = spi_rx_data & 0xFF;

            // Enqueue CAN message to be sent with data
            enqueue(&tx_message_queue, tx_data);
            print("Enqueued TX message:\n");
            print_hex_bytes(tx_data, 8);
        }
    }
}


void send_read_sensor_command(uint8_t field_number) {
    // Send the command to PAY-Optical to start reading data
    send_spi((0b11 << 6) | field_number);

    spi_rx_data_field_number = field_number;
    spi_rx_data = 0;
    spi_rx_data_in_progress = true;
    spi_rx_data_num_bytes_received = 0;
}
