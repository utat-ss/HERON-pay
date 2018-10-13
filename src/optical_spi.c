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

// For testing, set this to false to not add a message to the CAN queue when data is obtained
bool opt_spi_enable_can = true;

// true if we are in the process of receiving SPI data
bool spi_in_progress = false;
// The field number we are requesting from PAY-Optical
uint8_t spi_field_number = 0;
// Number of bytes already received from PAY-Optical
uint8_t spi_num_bytes_rcvd = 0;
// Data already received from PAY-Optical
uint32_t spi_data_rcvd = 0;


/*
Initializes the microcontroller to communicate with the PAY-Optical microcontroller over SPI.
*/
void opt_spi_init(void) {
    // set behaviour of INT1 to trigger on rising edge
    EICRA |= (1 << ISC10) | (1 << ISC11);
    // enable external interrupt 0
    EIMSK |= (1 << INT1);
    // enable global interrupts
    sei();

    // Initialize PAY-Optical CS pin
    init_cs(OPT_CS_PIN, &OPT_CS_DDR);
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // Initialize PAY-Optical reset pin
    init_cs(OPT_RST_PIN, &OPT_RST_DDR);
    set_cs_high(OPT_RST_PIN, &OPT_RST_PORT);
}

/*
Resets the PAY-Optical microcontroller.
*/
void opt_spi_rst(void) {
    // TODO - check how many cycles is necessary
    set_cs_low(OPT_RST_PIN, &OPT_RST_PORT);
    _delay_ms(1000);
    set_cs_high(OPT_RST_PIN, &OPT_RST_PORT);
}


/*
Sends a command to read data for the specified field number from PAY-Optical.
*/
void opt_spi_send_read_cmd(uint8_t field_number) {
    spi_in_progress = true;
    spi_field_number = field_number;
    spi_data_rcvd = 0;
    spi_num_bytes_rcvd = 0;

    // Send the command to PAY-Optical to start reading data
    print("Sending optical command - field #%u\n", field_number);
    set_cs_low(OPT_CS_PIN, &OPT_CS_PORT);
    uint8_t spi_tx = (0b11 << 6) | spi_field_number;
    send_spi(spi_tx);
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // After this, PAY-Optical should set DATA_RDY high, which will trigger the
    // interrupt below to receive the data
}


/*
This interrupt is triggered when the DATA_RDY pin goes set_cs_high
(set by PAY-Optical to indicate data is ready).
*/
ISR(INT1_vect) {
    // print("INT1 - DATA_RDY\n");

    // SPI should be in progress if we get this interrupt, but just check
    if (spi_in_progress) {
        // Get the next byte over SPI
        set_cs_low(OPT_CS_PIN, &OPT_CS_PORT);
        uint8_t new_byte = send_spi(0x00);
        set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

        // Add new byte to received data
        spi_data_rcvd = spi_data_rcvd << 8;
        spi_data_rcvd = spi_data_rcvd | new_byte;

        // Received one more byte
        spi_num_bytes_rcvd++;

        // If we have received all 3 bytes now
        if (spi_num_bytes_rcvd >= 3) {
            print ("Received optical data: %06lx\n", spi_data_rcvd);

            // If we have CAN enabled, enqueue the message to send data back to OBC
            // TODO
            if (opt_spi_enable_can) {
                // uint8_t tx_data[8];
                // tx_data[0] = 0; // TODO
                // tx_data[1] = CAN_PAY_SCI;
                // tx_data[2] = spi_field_number;
                //
                // tx_data[3] = (spi_data_rcvd >> 16) & 0xFF;
                // tx_data[4] = (spi_data_rcvd >> 8) & 0xFF;
                // tx_data[5] = spi_data_rcvd & 0xFF;
                //
                // // Enqueue CAN message to be sent with data
                // enqueue(&tx_message_queue, tx_data);
                // print("Enqueued TX\n");
                // print_bytes(tx_data, 8);
            }

            // Stop SPI requests
            spi_in_progress = false;
            spi_num_bytes_rcvd = 0;
            spi_data_rcvd = 0;
            spi_field_number = 0;
        }
    }
}
