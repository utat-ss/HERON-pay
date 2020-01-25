/*
Optical SPI

This handles the process of communicating with the microcontroller on the
PAY-Optical board. Using SPI, this microcontroller (PAY-SSM) functions as the master while
the PAY-Optical microcontroller functions as the slave.

Protocol:

PAY sends 2 bytes:
byte 1 = command opcode
byte 2 = well data (or other additional info)

PAY responds by interpreting the command opcode. OPTICAL performs the command.

Sometimes, PAY's requested command always requires data to be returned (usually 24-bit data from the ADC).
In this case, OPTICAL loads 1 byte of data into the SPI Data Register (SPDR).
It will then set its DATA_RDYn line LOW, signaling PAY to initiate a SPI transfer.
The data between PAY's and OPTICAL's SPDR registers are swapped. This is repeated as many times
as necessary until all the data is sent from OPTICAL to PAY.

The SPI Transmission Flag (SPIF) is set at the end of every SPI transmission. It is
automatically cleared when the SPDR register is accessed.

well_info interpreted as:
bit 7 - optical density = 0, fluorescent LED = 1
bit 4-0 = well number (0-31)


TODO:
- implement timeout
*/

#include "optical_spi.h"

// volatile needed?
volatile uint8_t spi_frame_number = 0;  // number of bytes received from OPTICAL
volatile uint32_t opt_spi_data = 0;     // Data received from OPTICAL, always right-aligned

// tracking if SPI in progress
bool spi_in_progress = false;

// set up slave select, reset, and DATA_RDYn pins
void init_opt_spi(void) {
    // Initialize PAY-Optical CS pin
    init_cs(OPT_CS_PIN, &OPT_CS_DDR );
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // Initialize PAY-Optical reset pin
    init_cs(OPT_RST_PIN, &OPT_RST_DDR);
    set_cs_high(OPT_RST_PIN, &OPT_RST_PORT);

    // Initialize PAY-Optical DATA_RDY pin
    OPT_DATA_DDR &= ~_BV(OPT_DATA);     // write 0, set as input pin
    OPT_DATA_PORT |= _BV(OPT_DATA);     // activate input pullup resistor

}


// Resets the PAY-Optical microcontroller
void rst_opt_spi(void) {
    // TODO - check how many cycles is necessary
    set_cs_low(OPT_RST_PIN, &OPT_RST_PORT);
    _delay_ms(1000);
    set_cs_high(OPT_RST_PIN, &OPT_RST_PORT);
}


// returns status of DATA_RDY pin
uint8_t get_data_pin(void){
    return OPT_DATA_PIN >> OPT_DATA;
}


// sends 2 bytes to request reading
// byte 1 = cmd_byte
// byte 2 = well_into
void send_opt_spi_cmd(uint8_t cmd_opcode, uint8_t well_info) {
    // print("Sending optical the command code %X\n", cmd_opcode);

    opt_spi_data = 0;
    spi_frame_number = 0;

    // Send the command to PAY-Optical to start reading data    
    set_cs_low(OPT_CS_PIN, &OPT_CS_PORT);
    send_spi(cmd_opcode);
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // wait for DATA_RDYn to go LOW, signaling optical is done reading byte 1
    // --> aka loop until DATA_RDYn is no longer HIGH
    uint16_t timeout = UINT16_MAX;
    while (get_data_pin() == 1 && timeout>0){
        timeout--;
    }

    // print("Sending optical the well data %X\n", well_info);
    // send PAY-OPTICAL the 2nd byte, containing details about the well and type of reading to take
    set_cs_low(OPT_CS_PIN, &OPT_CS_PORT);
    send_spi(well_info);
    set_cs_high(OPT_CS_PIN, &OPT_CS_PORT);

    // set SPI status to "waiting for OPTICAL to respond"
    spi_in_progress = true;
}


// function called by PAY-SSM in its main loop 'every once in a while' to check up on OPTICAL after it sent it a command
// if PAY-OPTICAL responded properly, proceed with multiple byte SPI transmission
// otherwise, do nothing
uint32_t check_received_opt_data(uint8_t num_expected_bytes){
    // if currently waiting for OPTICAL response
    if (spi_in_progress){
        // clear previously received data
        opt_spi_data = 0;

        // OPTICAL responded by pulling it's DATA_RDYn line low
        if (get_data_pin()==0){
            // exchange all the required bytes
            while (num_expected_bytes > spi_frame_number){
                // OPTICAL ready to send data
                uint8_t new_opt_data = SPDR;
                opt_spi_data = opt_spi_data<<8 | new_opt_data;

                spi_frame_number++;
            }
        }

        // successfully sent command, and received all bytes from OPTICAL
        spi_in_progress = false;

        return opt_spi_data;
    }

    else // no response from OPTICAL -> do nothing
        return 0;
}


// returns number of expected bytes to received from SPI, based on opcode cmd
uint8_t opcode_to_num_bytes(uint8_t opcode){
    switch (opcode){
        case CMD_GET_READING:
            return NUM_GET_READING;
        default:
            return 0;
    }
}