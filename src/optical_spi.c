/*
Optical SPI

This handles the process of communicating with the microcontroller on the
PAY-Optical board. Using SPI, this microcontroller (PAY) functions as the master while
the PAY-Optical microcontroller functions as the slave.

PAY requests data for a specific field number, then PAY-Optical reads that data
and sends it back.


Protocol:

PAY sends 1 byte
Bits 7-6: if 11, this is a request
Bits 5-0: the "field number", meaning which sensor to read (currently 0-33, maybe 0-47 later?)

Repeat 3 times:
PAY-Optical retrieves the data and sets the DATA_RDY pin high
PAY interrupt triggered from the DATA_RDY pin
PAY initiates a 1 byte SPI transfer to get the data

PAY now has the 24 bit ADC reading (raw data) from PAY-Optical, which was sent
as big-endian/network order (most significant byte first)


TODO - maybe put protocol and constants in lib-common?
*/

#include "optical_spi.h"

bool spi_in_progress = false;
uint8_t spi_num_bytes_rcvd = 0; // number of bytes already received from PAY-Optical
uint32_t spi_data_rcvd = 0;
uint8_t spi_field_number = 0;

void ssm_spi_init(void) {
	// set behaviour of INT1 to trigger on rising edge
	EICRA |= (1 << ISC10) | (1 << ISC11);
	// enable external interrupt 0
	EIMSK |= (1 << INT1);
	// enable global interrupts
	sei();
}

void opt_spi_init(void) {
    init_cs(OPTICAL_CS_PIN, &OPTICAL_CS_DDR);
    set_cs_high(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);

    init_cs(OPTICAL_RST_PIN, &OPTICAL_RST_DDR);
    set_cs_high(OPTICAL_RST_PIN, &OPTICAL_RST_PORT);
}


void opt_spi_rst(void) {
    // TODO - check how many cycles is necessary
    set_cs_low(OPTICAL_RST_PIN, &OPTICAL_RST_PORT);
    _delay_ms(1000);
    set_cs_high(OPTICAL_RST_PIN, &OPTICAL_RST_PORT);
}


void opt_spi_send_read_cmd(uint8_t field_number) {
    spi_field_number = field_number;
    spi_data_rcvd = 0;
    spi_num_bytes_rcvd = 0;
    spi_in_progress = true;

    // Send the command to PAY-Optical to start reading data
    set_cs_low(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
    send_spi((0b11 << 6) | spi_field_number);
    set_cs_high(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);

    print("Sent SPI command to PAY-Optical - field %u\n", field_number);
}


ISR(INT1_vect) {
    print("Interrupt - INT1 - DATA_RDY - PB2 from PAY-Optical\n");

    if (spi_in_progress) {
        set_cs_low(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
        uint8_t new_byte = send_spi(0x00);
        set_cs_high(OPTICAL_CS_PIN, &OPTICAL_CS_PORT);
        print("Received byte %02x\n", new_byte);

        spi_data_rcvd = spi_data_rcvd << 8;
        spi_data_rcvd = spi_data_rcvd | new_byte;

        spi_num_bytes_rcvd++;
        if (spi_num_bytes_rcvd >= 3) {
            spi_in_progress = false;
            spi_num_bytes_rcvd = 0;

            print ("Received data from PAY-Optical: %lu\n", spi_data_rcvd);
            spi_data_rcvd = 0;

            // TODO - enable, refactor into function, move to separate CAN file
            // uint8_t tx_data[8];
            // tx_data[0] = 0; // TODO
            // tx_data[1] = CAN_PAY_SCI;
            // tx_data[2] = spi_field_number;
            //
            // tx_data[3] = (spi_rx_data >> 16) & 0xFF;
            // tx_data[4] = (spi_rx_data >> 8) & 0xFF;
            // tx_data[5] = spi_rx_data & 0xFF;
            //
            // // Enqueue CAN message to be sent with data
            // enqueue(&tx_message_queue, tx_data);
            // print("Enqueued TX\n");
            // print_hex_bytes(tx_data, 8);
        }
    }
}
