/*

    FILENAME:            main.c
    DEPENDENCIES:        pex, uart, can, queue, freq_measure, stdint, delay

    DESCRIPTION:        Main functions to be implemented on the PAY 32M1
    AUTHORS:            Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
    DATE MODIFIED:      2018-02-24
    NOTE:
    PLEASE DON'T COMMIT YOUR TESTING CODE
    ONLY COMMIT ACTUAL CHANGES TO MAIN.C

    REVISION HISTORY:

    2018-01-08:     DV: Added CAN id definitions to the header file
    2017-11-20:     SS: Added testing for PAY command queue
    2017-11-17:     Added "sensor_led_sequence()" as a sanity check
    2017-11-16:     Created header. Implemented 'separate function for testing' rule

*/

// TODO - consider implementing function error checking (e.g. return 1 for success, 0 for failure)

#include "main.h"

void print_bytes(uint8_t *data, uint8_t len);
uint32_t read_optical_raw(int channel, int LED);
void handle_rx_hk(uint8_t* tx_data);
void handle_rx_sci(uint8_t* tx_data);
void handle_rx(void);
void setup_adc(void);
void init_pay(void);


// CAN messages received but not processed yet
queue_t rx_message_queue;
// CAN messages to transmit
queue_t tx_message_queue;




// Prints the given data in hex format, with a space between bytes
void print_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
}


// Gets the raw 24 bit optical measurement using the given ADC channel and LED
// (quickly pulses the LED and reads the optical measurement)
uint32_t read_optical_raw(int channel, int LED) {
    print("Getting optical data: channel %d, LED %d\n", channel, LED);

    // Turn LED on
    set_gpio_a(SENSOR_PCB, LED);
    _delay_ms(10);

    // Read sensor
    uint32_t read_data = read_ADC_channel(channel);
    // print("%d:  %lu  %lX\t  %lu\n",
    //             // i, read_data * ((uint32_t)1000) / ((uint32_t)0xFFFFFF), read_data, read_data);
    //             i, (uint32_t)(((double)read_data / (double)0xFFFFFF) * 1000.0) , read_data, read_data);

    // Turn LED off
    // BUG: Shouldn't 'hard reset' the PEX in final implementation
    clear_gpio_a(SENSOR_PCB, LED);
    adc_pex_hard_rst();

    print("Done getting optical data\n");

    return read_data;
}




// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_rx_hk(uint8_t* tx_data) {
    // Check field number
    switch (tx_data[1]) {
        case PAY_TEMP_1:
            print("PAY_TEMP_1\n");

            // TODO - fix issues with temperature sometimes reading 0
            print("Getting temperature\n");

            uint16_t raw_temperature = 0;
            int attempts = 0;
            for (attempts = 0; raw_temperature == 0; attempts++) {
                raw_temperature = read_raw_temperature();
            }

            print("Done getting temperature: %d attempts\n", attempts);

            tx_data[2] = 0x00;
            tx_data[3] = (raw_temperature >> 8) & 0xFF;
            tx_data[4] = raw_temperature & 0xFF;

            break;

        // TODO
        // TODO - temperature needed for pressure calculation
        case PAY_PRES_1:
            print("PAY_PRES_1\n");

            uint32_t d1 = read_raw_pressure();
            uint32_t d2 = read_raw_pressure_temp();
            int32_t pressure = convert_pressure_to24bits(PROM_data, d1, d2);

            tx_data[2] = (pressure >> 16) & 0xFF;
            tx_data[3] = (pressure >> 8) & 0xFF;
            tx_data[4] = pressure & 0xFF;

            break;

        case PAY_HUMID_1:
            print("PAY_HUMID_1\n");

            // read_raw_humidity() gives 16 bits of humidity followed by 16 bits of temperature,
			// but humidity is a 14 bit value
            print("Getting humidity\n");
            uint32_t raw_humidity = read_raw_humidity();
            print("Done getting humidity\n");

            // Use the most significant (left-most) 16 bits, which are humidity
            tx_data[2] = 0x00;
            tx_data[3] = (raw_humidity >> 24) & 0xFF;
            tx_data[4] = (raw_humidity >> 16) & 0xFF;

            break;

        // TODO
        case PAY_MF_TEMP_1:
            print("PAY_MF_TEMP_1\n");
            break;
        case PAY_MF_TEMP_2:
            print("PAY_MF_TEMP_2\n");
            break;
        case PAY_MF_TEMP_3:
            print("PAY_MF_TEMP_3\n");
            break;

        default:
            print("Unknown housekeeping field number\n");
            break;
    }
}


// Assuming a science request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_rx_sci(uint8_t* tx_data) {
    // For sensor to poll
    int channel = 0;
    int LED = 0;

	// Check field number
	switch (tx_data[1]) {
        case PAY_SCI_TEMD:
            print("PAY_SCI_TEMD\n");
        	channel = 5;
        	LED = LED_2;
            break;

        case PAY_SCI_SFH:
            print("PAY_SCI_SFH\n");
        	channel = 6;
        	LED = LED_3;
            break;

        default:
            print ("Unknown science field number\n");
            return;
	}

    // Random dummy value
    // uint32_t reading = rand() % ((uint32_t) 1 << 16);
    // reading |= (rand() % ((uint32_t) 1 << 8)) << 16;

    // Actual value
    uint32_t optical_reading = read_optical_raw(channel, LED);

    // Constant value
    // uint32_t optical_reading = 0x00010203;

	tx_data[2] = (optical_reading >> 16) & 0xFF;
	tx_data[3] = (optical_reading >> 8) & 0xFF;
	tx_data[4] = optical_reading & 0xFF;
}


void handle_rx(void) {
    if (is_empty(&rx_message_queue)) {
        print("No data in RX message queue\n");
        return;
    }

    // Received message
    uint8_t rx_data[8];
    dequeue(&rx_message_queue, rx_data);
    print("Dequeued RX Message\n");
    print_bytes(rx_data, 8);

    // Message to transmit
    uint8_t tx_data[8];

    // Send back the message type and field number
    tx_data[0] = rx_data[0];
    tx_data[1] = rx_data[1];

    // Fill the rest with zeros just in case
    for (uint8_t i = 2; i < 8; i++) {
        tx_data[i] = 0;
    }

    // Check message type
    switch (rx_data[0]) {
        case PAY_HK_REQ:
            print("PAY_HK_REQ\n");
            handle_rx_hk(tx_data);
            break;

        case PAY_SCI_REQ:
            print("PAY_SCI_REQ\n");
            handle_rx_sci(tx_data);
            break;

        // TODO
        case PAY_HEATER_REQ:
            print("PAY_HEATER_REQ\n");
            break;

        default:
            print("Unknown message type\n");
            break;
    }

    // TODO - should it not transmit if the received message is not recognized?

    // Enqueue TX data to transmit
    enqueue(&tx_message_queue, tx_data);
    print("Enqueued TX Message\n");
    print_bytes(tx_data, 8);
}




/* CAN Interrupts */


// MOB 0
// For heartbeat
void status_rx_callback(const uint8_t* data, uint8_t len) {
    print("MOB 0: Status RX Callback\n");
    print("Received Message:\n");
    print_bytes((uint8_t *) data, len);
}


// MOB 1
// For heartbeat
void status_tx_callback(uint8_t* data, uint8_t* len) {
    print("MOB 1: Status TX Callback\n");
}


// MOB 2
// For later science requests?
void cmd_tx_callback(uint8_t* data, uint8_t* len) {
    print("MOB 2: CMD TX Callback\n");
}


// MOB 3
// CAN RX interrupt for received commands
void cmd_rx_callback(const uint8_t* data, uint8_t len) {
    print("\n\n\n\nMOB 3: CMD RX Callback\n");
    print("Received Message:\n");
    print_bytes((uint8_t *) data, len);

    // TODO - would this ever happen?
    if (len == 0) {
        print("Received empty message\n");
    }

    // If the RX message exists, add it to the queue of received messages to process
    else {
        enqueue(&rx_message_queue, (uint8_t *) data);
        print("Enqueued RX message");
    }
}


// MOB 5
// CAN TX interrupt for sending data
void data_tx_callback(uint8_t* data, uint8_t* len) {
    print("\nData TX Callback\n");

    // TODO - would this ever happen?
    if (is_empty(&tx_message_queue)) {
        *len = 0;

        print("No message to transmit\n");
    }

    // If there is a message in the TX queue, transmit it
    else {
        dequeue(&tx_message_queue, data);
        *len = 8;

        print("Dequeued TX Message\n");
        print("Transmitting Message:\n");
        print_bytes(data, *len);
    }
}




void setup_adc(void) {
	init_port_expander();
	init_adc();
	print("ADC Setup Completed\n");

	write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);
    uint32_t config_data = read_ADC_register(CONFIG_ADDR);
	print("ADC Configuration Register: %lX\n", config_data);

    int pga_gain = 1;
	set_PGA(pga_gain);
	print("PGA gain: %d\n", pga_gain);
}


// Initializes everything in PAY
void init_pay(void) {
    // UART
    init_uart();
    print("\n\nUART Initialized\n");

    // SPI and sensors
    init_spi();
    sensor_setup();
    print("SPI and Sensors Initialized\n");

    // ADC
    setup_adc();
    print("ADC Initialized\n");

    // CAN and MOBs
    init_can();
    init_rx_mob(&status_rx_mob);
    init_tx_mob(&status_tx_mob);
    init_tx_mob(&cmd_tx_mob);
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
    print("PAY Initialized\n\n");

    // Main loop
    while (1) {
        // If there is an RX messsage in the queue, handle it
        if (!is_empty(&rx_message_queue)) {
            handle_rx();
        }

        /*
        If there is a TX message in the queue, send it
        TODO - should new data already be available to send instead of waiting for it?

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
