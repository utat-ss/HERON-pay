/*
Optical SPI

This handles the process of communicating with the microcontroller on the
PAY-Optical board. Using SPI, this microcontroller (PAY-SSM) functions as the master while
the PAY-Optical microcontroller functions as the slave.

Protocol:

PAY sends 2 bytes:
byte 1 = command opcode
byte 2 = well info (or other additional info)

PAY responds by interpreting the command opcode. OPTICAL performs the command.

Sometimes, PAY's requested command always requires data to be returned (usually 24-bit data from the ADC).
In this case, OPTICAL loads 1 byte of data into the SPI Data Register (SPDR).
It will then set its DATA_RDYn line LOW, signaling PAY to initiate a SPI transfer.
The data between PAY's and OPTICAL's SPDR registers are swapped. This is repeated as many times
as necessary until all the data is sent from OPTICAL to PAY.

The SPI Transmission Flag (SPIF) is set at the end of every SPI transmission. It is
automatically cleared when the SPDR register is accessed.

well_info interpreted as:
bit 5 - optical density = 0, fluorescent = 1
bit 4-0 = well number (0-31)
*/

#include "optical_spi.h"

// tracking if SPI in progress
bool spi_in_progress = false;
uint8_t current_well_info = 0;

bool print_spi_transfers = true;


// set up slave select, reset, and DATA_RDYn pins
void init_opt_spi(void) {
    // Initialize PAY-Optical CS pin
    init_cs(OPT_CS, &OPT_CS_DDR );
    set_cs_high(OPT_CS, &OPT_CS_PORT);

    // Initialize PAY-Optical reset pin
    init_cs(OPT_RST, &OPT_RST_DDR);
    set_cs_high(OPT_RST, &OPT_RST_PORT);

    // Initialize PAY-Optical DATA_RDY pin
    OPT_DATA_DDR &= ~_BV(OPT_DATA);     // write 0, set as input pin
    OPT_DATA_PORT |= _BV(OPT_DATA);     // activate input pullup resistor
}


// Resets the PAY-Optical microcontroller
void rst_opt_spi(void) {
    // TODO - check how many cycles is necessary
    set_cs_low(OPT_RST, &OPT_RST_PORT);
    _delay_ms(1000);
    set_cs_high(OPT_RST, &OPT_RST_PORT);
}


// returns status of DATA_RDY pin
uint8_t get_data_pin(void){
    return (OPT_DATA_PIN >> OPT_DATA) & 0x1;
}


// sends 2 bytes to request reading
// byte 1 = cmd_byte
// byte 2 = well_into
void send_opt_spi_cmd(uint8_t cmd_opcode, uint8_t well_info) {
    uint8_t tx_bytes[2] = {0x00};
    tx_bytes[0] = cmd_opcode;
    tx_bytes[1] = well_info;

    if (print_spi_transfers) {
        print("SPI TX: ");
        print_bytes(tx_bytes, 2);
    }

    // Send the command to PAY-Optical to start reading data    
    set_cs_low(OPT_CS, &OPT_CS_PORT);
    send_spi(tx_bytes[0]);
    set_cs_high(OPT_CS, &OPT_CS_PORT);

    _delay_ms(10);

    // send PAY-OPTICAL the 2nd byte, containing details about the well and type of reading to take
    set_cs_low(OPT_CS, &OPT_CS_PORT);
    send_spi(tx_bytes[1]);
    set_cs_high(OPT_CS, &OPT_CS_PORT);

    // Need to give optical time to deassert and assert DATA_RDY
    _delay_ms(100);
}

uint32_t get_opt_spi_resp(void) {
    uint8_t rx_bytes[OPT_SPI_RX_COUNT] = {0x00};

    // exchange all the required bytes
    for (uint8_t i = 0; i < OPT_SPI_RX_COUNT; i++) {
        set_cs_low(OPT_CS, &OPT_CS_PORT);
        rx_bytes[i] = send_spi(0x00);
        set_cs_high(OPT_CS, &OPT_CS_PORT);

        // small delay to give optical time to get SPDR again
        _delay_ms(100);
    }

    if (print_spi_transfers) {
        print("SPI RX: ");
        print_bytes(rx_bytes, OPT_SPI_RX_COUNT);
    }

    // Data received from OPTICAL, always right-aligned
    uint32_t rx_data =
        ((uint32_t) rx_bytes[0] << 16) |
        ((uint32_t) rx_bytes[1] << 8) |
        ((uint32_t) rx_bytes[2] << 0);
    return rx_data;
}

void start_opt_spi_get_reading(uint8_t well_info) {
    send_opt_spi_cmd(CMD_GET_READING, well_info);

    // set SPI status to "waiting for OPTICAL to respond"
    spi_in_progress = true;
    current_well_info = well_info;
}

// function called by PAY-SSM in its main loop 'every once in a while' to check up on OPTICAL after it sent it a command
// if PAY-OPTICAL responded properly, proceed with multiple byte SPI transmission
// otherwise, do nothing
void check_opt_spi_get_reading(void){
    // if currently waiting for OPTICAL response
    if (spi_in_progress){
        // OPTICAL responded by pulling its DATA_RDYn line low
        if (get_data_pin()==0){
            // print("Found DATA_RDYn low\n");

            uint32_t opt_spi_data = get_opt_spi_resp();

            // successfully sent command, and received all bytes from OPTICAL
            spi_in_progress = false;
        
            // create CAN message
            uint8_t tx_msg[8] = { 0x00 };
            tx_msg[0] = CAN_PAY_OPT;            // opcode
            tx_msg[1] = current_well_info;      // field number
            tx_msg[2] = CAN_STATUS_OK;          // status, 0x00 = ok
            tx_msg[3] = 0x00;                   // unused
            tx_msg[4] = 0x00;                   // data bits 4-7 read as uint32_t
            tx_msg[5] = (opt_spi_data >> 16) & 0xFF; 
            tx_msg[6] = (opt_spi_data >> 8) & 0xFF;
            tx_msg[7] = (opt_spi_data) & 0xFF;
            // Enqueue TX message to transmit
            enqueue(&tx_msg_queue, tx_msg);
        }
    }

    // no response from OPTICAL -> do nothing
}

// For synchronous commands (all except for get reading)
uint32_t run_opt_spi_sync_cmd(uint8_t cmd_opcode, uint8_t well_info) {
    send_opt_spi_cmd(cmd_opcode, well_info);

    // Wait for DATA_RDYn to go low
    uint32_t timeout = 1000;
    while (get_data_pin() && timeout > 0){
        timeout--;
        _delay_ms(1);
    }
    if (timeout == 0) {
        print("TIMEOUT in run_opt_spi_sync_cmd\n");
    }

    return get_opt_spi_resp();
}
