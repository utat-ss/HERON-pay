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

// Must use volatile variables because we modify them in the ISR, but read them
// from the blocking function

// Number of bytes already received from PAY-Optical
volatile uint8_t opt_spi_num_bytes = 0;
// Data already received from PAY-Optical, always right-aligned
volatile uint32_t opt_spi_data = 0;


/*
Initializes the microcontroller to communicate with the PAY-Optical microcontroller over SPI.
*/
void init_opt_spi(void) {
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
void rst_opt_spi(void) {
    // TODO - check how many cycles is necessary
    set_cs_low(OPT_RST_PIN, &OPT_RST_PORT);
    _delay_ms(1000);
    set_cs_high(OPT_RST_PIN, &OPT_RST_PORT);
}

// Reads 24 bits of raw data from the PAY-Optical microcontroller over optical SPI
// (Sends the request command, then blocks until it gets 3 bytes back)
uint32_t read_opt_spi(uint8_t field_num) {
    // Must make this block non-atomic to ensure interrupts are enabled, in case
    // this function executes within an atomic block (e.g. in main() of main_test)
    NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE) {
        send_opt_spi_cmd(field_num);

        // Wait until we get 3 bytes, with a timeout
        // In testing, timeout always ended at the original value minus 7 or 8
        uint8_t timeout = UINT8_MAX;
        while ((timeout > 0) && (opt_spi_num_bytes < 3)) {
            timeout--;
            timeout++;
        }
        // print("timeout = %u\n", timeout);

        // If we didn't get 3 bytes
        if (opt_spi_num_bytes < 3) {
            //_delay_ms(5000); // long delay = timeout, used for testing
            return 0;
        }

        // If we have received all 3 bytes now
        uint32_t read_data = opt_spi_data;
        // print ("Received optical data: 0x%06lx\n", read_data);

        // Clear SPI request variables
        opt_spi_num_bytes = 0;
        opt_spi_data = 0;

        return read_data;
    }

    // Silence missing return warning
    return 0;
}

/*
Sends a command to read data for the specified field number from PAY-Optical.
*/
void send_opt_spi_cmd(uint8_t field_num) {
    // print("Sending optical command - field #%u\n", field_num);

    opt_spi_data = 0;
    opt_spi_num_bytes = 0;

    // Send the command to PAY-Optical to start reading data
    set_cs_low(OPT_CS_PIN, &OPT_CS_PORT);
    uint8_t spi_tx = (0b11 << 6) | field_num;
    send_spi(spi_tx);
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // After this, PAY-Optical should set DATA_RDY high, which will trigger the
    // interrupt below to receive the data
}


/*
This interrupt is triggered when the DATA_RDY pin goes set_cs_high
(set by PAY-Optical to indicate data is ready).
The SPI transfer should already be in progress if we get this interrupt.
*/
ISR(INT1_vect) {
    // print("INT1 - DATA_RDY\n");

    // Get the next byte over SPI
    set_cs_low(OPT_CS_PIN, &OPT_CS_PORT);
    uint8_t new_byte = send_spi(0x00);
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // Add new byte to received data
    opt_spi_data = (opt_spi_data << 8) | new_byte;
    // Received one more byte
    opt_spi_num_bytes++;
}
