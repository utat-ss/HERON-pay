/*
Main functions to be implemented on the PAY 32M1

AUTHORS: Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown

NOTE:
PLEASE DON'T COMMIT YOUR TESTING CODE
ONLY COMMIT ACTUAL CHANGES TO MAIN.C
*/

#include "main.h"

uint8_t spi_rx_data_field_number = 0;
uint32_t spi_rx_data = 0;
bool spi_rx_data_in_progress = false;
uint8_t spi_rx_data_num_bytes_received = 0; // number of bytes already received from PAY-Optical
bool spi_rx_data_valid = false;


void send_read_sensor_command(uint8_t field_number) {
    // Send the command to PAY-Optical to start reading data
    send_spi((0b11 << 6) | field_number);

    spi_rx_data_field_number = field_number;
    spi_rx_data = 0;
    spi_rx_data_in_progress = true;
    spi_rx_data_num_bytes_received = 0;
    spi_rx_data_valid = false;
}

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
            spi_rx_data_valid = true;

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


// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_rx_hk(uint8_t* tx_data) {
    // Check field number
    switch (tx_data[2]) {
        case CAN_PAY_HK_TEMP:
            print("CAN_PAY_HK_TEMP\n");

            uint16_t temp_raw_data = temp_read_raw_data();
            tx_data[3] = 0x00;
            tx_data[4] = (temp_raw_data >> 8) & 0xFF;
            tx_data[5] = temp_raw_data & 0xFF;

            break;

        case CAN_PAY_HK_HUMID:
            print("CAN_PAY_HK_HUMID\n");

            uint16_t raw_humidity = hum_read_raw_humidity();
            tx_data[3] = 0x00;
            tx_data[4] = (raw_humidity >> 8) & 0xFF;
            tx_data[5] = raw_humidity & 0xFF;

            break;

        case CAN_PAY_HK_PRES:
            print("CAN_PAY_HK_PRES\n");

            uint32_t D1 = pres_read_raw_uncompensated_pressure();
            uint32_t D2 = pres_read_raw_uncompensated_temperature();
            uint32_t raw_pressure = pres_convert_raw_uncompensated_data_to_raw_pressure(D1, D2, NULL);

            tx_data[3] = (raw_pressure >> 16) & 0xFF;
            tx_data[4] = (raw_pressure >> 8) & 0xFF;
            tx_data[5] = raw_pressure & 0xFF;

            break;

        default:
            print("Unknown housekeeping field number\n");
            break;
    }
}


void handle_rx(void) {
    if (is_empty(&rx_message_queue)) {
        print("No data in RX message queue\n");
        return;
    }

    // Received message
    uint8_t rx_data[8];
    dequeue(&rx_message_queue, rx_data);
    print("Dequeued RX message:\n");
    print_hex_bytes(rx_data, 8);

    // Message to transmit
    uint8_t tx_data[8];

    // Send back the same message type and field number
    tx_data[0] = 0; // TODO
    tx_data[1] = rx_data[1];
    tx_data[2] = rx_data[2];

    // Fill the rest with zeros just in case
    for (uint8_t i = 3; i < 8; i++) {
        tx_data[i] = 0;
    }

    // Check message type
    switch (rx_data[1]) {
        case CAN_PAY_HK:
            print("CAN_PAY_HK\n");
            handle_rx_hk(tx_data);
            break;

        case CAN_PAY_SCI:
            print("CAN_PAY_SCI\n");
            send_read_sensor_command(rx_data[2]);
            return;

        case CAN_PAY_MOTOR:
            print("CAN_PAY_MOTOR\n");
            if (rx_data[2] == CAN_PAY_MOTOR_ACTUATE) {
                actuate_motors();
            }
            break;

        default:
            print("Unknown message type\n");
            break;
    }

    // Enqueue TX data to transmit
    enqueue(&tx_message_queue, tx_data);
    print("Enqueued TX message:\n");
    print_hex_bytes(tx_data, 8);
}




// Initializes everything in PAY
void init_pay(void) {
    // UART
    init_uart();
    print("\n\nUART Initialized\n");

    // SPI
    init_spi();
    print("SPI Initialized\n");

    // PEX
    pex_init_constants(PEX_PAY);
    pex_init();
    print("PEX Initialized\n");

    // ADC
    adc_init_constants(ADC_PAY);
    adc_init();
    print("ADC Initialized\n");

    // Motors
    init_motors();
    print("Motors Initialized\n");

    // Environmental sensors
    temp_init();
    hum_init();
    pres_init();
    print("Environmental Sensors Initialized\n");

    // CAN and MOBs
    init_can();
    init_rx_mob(&cmd_rx_mob);
    init_tx_mob(&data_tx_mob);
    print("CAN Initialized\n");

    // Queues
    init_queue(&rx_message_queue);
    init_queue(&tx_message_queue);
    print("Queues Initialized\n");
}


int main(void) {
    init_pay();
    print("---------------\n");
    print("PAY Initialized\n\n");


    // Main loop
    print("Starting main loop\n\n");
    while (1) {
        // If there is an RX messsage in the queue, handle it
        if (!is_empty(&rx_message_queue)) {
            handle_rx();
        }

        /*
        If there is a TX message in the queue, send it

        When resume_mob(mob name) is called, it:
        1) resumes the MOB
        2) triggers an interrupt (callback function) to get the data to transmit
        3) sends the data
        4) pauses the mob
        */
        if (!is_empty(&tx_message_queue)) {
            resume_mob(&data_tx_mob);
        }
    }
}

// TODO - should this be INT2 or PCINT2?
ISR(PCINT2_vect) {
    print("Interrupt - PCINT2 (Pin change interrupt 2, INTA from PEX) interrupt!\n");

    // GPA0 = _FLT_A_
    // GPA1 = _FLT_B_

    // Check if either of the motor _FLT_ (fault) pins is low
    uint8_t gpioa = pex_read(PEX_GPIO_BASE);
    if ((gpioa & _BV(0)) == 0) {
        motor_fault = true;
        print("MOTOR A FAULT DETECTED!\n");
    }
    if ((gpioa & _BV(1)) == 0) {
        motor_fault = true;
        print("MOTOR B FAULT DETECTED!\n");
    }

    if (motor_fault) {
        disable_motors();
    }
}
