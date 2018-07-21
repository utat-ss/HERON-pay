// #include "optical.h"
//
// bool spi_rx_data_in_progress = false;
// uint8_t spi_rx_data_num_bytes_received = 0; // number of bytes already received from PAY-Optical
// uint32_t spi_rx_data = 0;
// uint8_t spi_rx_data_field_number = 0;
//
//
// void init_optical(void) {
//     init_cs(OPTICAL_CS_PIN, &OPTICAL_CS_DDR);
//     set_cs_high(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
//
//     init_cs(OPTICAL_RST_PIN, &OPTICAL_RST_DDR);
//     set_cs_high(OPTICAL_RST_PIN, &OPTICAL_RST_PORT);
// }
//
//
// void rst_optical(void) {
//     // TODO - check how many cycles is necessary
//     set_cs_low(OPTICAL_RST_PIN, &OPTICAL_RST_PORT);
//     _delay_ms(1000);
//     set_cs_high(OPTICAL_RST_PIN, &OPTICAL_RST_PORT);
// }
//
//
// void send_read_optical_command(uint8_t field_number) {
//     spi_rx_data_field_number = field_number;
//     spi_rx_data = 0;
//     spi_rx_data_num_bytes_received = 0;
//     spi_rx_data_in_progress = true;
//
//     // Send the command to PAY-Optical to start reading data
//     set_cs_low(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
//     send_spi((0b11 << 6) | field_number);
//     set_cs_high(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
//
//     print("Sent SPI command to PAY-Optical - field %u\n", field_number);
// }
//
//
// // TODO - INT1 or PCINT1?
// // PB2/INT1 interrupt - DATA_RDY pin from PAY-Optical
// ISR(INT1_vect) {
//     print("Interrupt - DATA_RDY - INT1 (PB2, from PAY-Optical)\n");
//
//     // TODO - is it possible to send SPI inside an interrupt?
//
//     if (spi_rx_data_in_progress) {
//         set_cs_low(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
//         uint8_t new_byte = send_spi(0x00);
//         set_cs_high(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
//         print("Received byte %02x\n", new_byte);
//
//         spi_rx_data = spi_rx_data << 8;
//         spi_rx_data = spi_rx_data | new_byte;
//         spi_rx_data_num_bytes_received++;
//
//         if (spi_rx_data_num_bytes_received >= 3) {
//             spi_rx_data_in_progress = false;
//             spi_rx_data_num_bytes_received = 0;
//
//             print("Received data from PAY-Optical: %06x = %d %%\n",
//                     spi_rx_data, (int8_t) ((double) spi_rx_data / (double) 0xFFFFFF * 100.0));
//
// #ifndef DISABLE_CAN
//             uint8_t tx_data[8];
//             tx_data[0] = 0; // TODO
//             tx_data[1] = CAN_PAY_SCI;
//             tx_data[2] = spi_rx_data_field_number;
//
//             tx_data[3] = (spi_rx_data >> 16) & 0xFF;
//             tx_data[4] = (spi_rx_data >> 8) & 0xFF;
//             tx_data[5] = spi_rx_data & 0xFF;
//
//             // Enqueue CAN message to be sent with data
//             enqueue(&tx_message_queue, tx_data);
//             print("Enqueued TX\n");
//             print_hex_bytes(tx_data, 8);
// #endif
//         }
//     }
// }
