/*
Main functions to be implemented on the PAY 32M1

AUTHORS: Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown

NOTE:
PLEASE DON'T COMMIT YOUR TESTING CODE
ONLY COMMIT ACTUAL CHANGES TO MAIN.C

TODO - consider implementing function error checking
(e.g. return 1 for success, 0 for failure)
*/

#include "main.h"

uint32_t read_optical_raw(int channel, int LED);
void handle_rx_hk(uint8_t* tx_data);
void handle_rx_sci(uint8_t* tx_data);
void handle_rx(void);
void setup_adc(void);
void init_pay(void);







// Gets the raw 24 bit optical measurement using the given ADC channel and LED
// (quickly pulses the LED and reads the optical measurement)
uint32_t read_optical_raw(int channel, int LED) {
    // TODO
    // print("Getting optical data: channel %d, LED %d\n", channel, LED);
    //
    // // Turn LED on
    // set_gpio_a(SENSOR_PCB, LED);
    // _delay_ms(10);
    //
    // // Read sensor
    // uint32_t read_data = read_ADC_channel(channel);
    // // print("%d:  %lu  %lX\t  %lu\n",
    // //             // i, read_data * ((uint32_t)1000) / ((uint32_t)0xFFFFFF), read_data, read_data);
    // //             i, (uint32_t)(((double)read_data / (double)0xFFFFFF) * 1000.0) , read_data, read_data);
    //
    // // Turn LED off
    // // BUG: Shouldn't 'hard reset' the PEX in final implementation
    // clear_gpio_a(SENSOR_PCB, LED);
    // adc_pex_hard_rst();
    //
    // print("Done getting optical data\n");

    uint32_t read_data;
    return read_data;
}




// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_rx_hk(uint8_t* tx_data) {
    // Check field number
    switch (tx_data[2]) {
        case CAN_PAY_HK_TEMP:
          print("CAN_PAY_HK_TEMP\n");


            // TODO - fix issues with temperature sometimes reading 0
            print("Getting temperature\n");

            uint16_t raw_temperature = 0;
            // TODO
            // int attempts = 0;
            // for (attempts = 0; raw_temperature == 0; attempts++) {
            //     raw_temperature = read_raw_temperature();
            // }
            // print("Done getting temperature: %d attempts\n", attempts);

            tx_data[3] = 0x00;
            tx_data[4] = (raw_temperature >> 8) & 0xFF;
            tx_data[5] = raw_temperature & 0xFF;

            break;

        // TODO
        // TODO - temperature needed for pressure calculation
        case CAN_PAY_HK_PRES:
            print("CAN_PAY_HK_PRES\n");

            // TODO
            // uint32_t d1 = read_raw_pressure();
            // uint32_t d2 = read_raw_pressure_temp();
            // int32_t pressure = convert_pressure_to24bits(PROM_data, d1, d2);
            int32_t pressure = 0;

            tx_data[3] = (pressure >> 16) & 0xFF;
            tx_data[4] = (pressure >> 8) & 0xFF;
            tx_data[5] = pressure & 0xFF;

            break;

        case CAN_PAY_HK_HUMID:
            print("CAN_PAY_HK_HUMID\n");

            // read_raw_humidity() gives 16 bits of humidity followed by 16 bits of temperature,
            // but humidity is a 14 bit value
            // TODO
            // print("Getting humidity\n");
            // uint32_t raw_humidity = read_raw_humidity();
            // print("Done getting humidity\n");
            uint32_t raw_humidity = 0;

            // Use the most significant (left-most) 16 bits, which are humidity
            tx_data[3] = 0x00;
            tx_data[4] = (raw_humidity >> 24) & 0xFF;
            tx_data[5] = (raw_humidity >> 16) & 0xFF;

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


    // TODO
    // // Check field number
    // switch (tx_data[1]) {
    //     case CAN_PAY_SCI_TEMD:
    //         print("CAN_PAY_SCI_TEMD\n");
    //         channel = 5;
    //         LED = LED_2;
    //         break;
    //
    //     case CAN_PAY_SCI_SFH:
    //         print("CAN_PAY_SCI_SFH\n");
    //         channel = 6;
    //         LED = LED_3;
    //         break;
    //
    //     default:
    //         print ("Unknown science field number\n");
    //         return;
    // }

    // Random dummy value
    // uint32_t reading = rand() % ((uint32_t) 1 << 16);
    // reading |= (rand() % ((uint32_t) 1 << 8)) << 16;

    // Actual value
    uint32_t optical_reading = read_optical_raw(channel, LED);

    // Constant value
    // uint32_t optical_reading = 0x00010203;

    tx_data[3] = (optical_reading >> 16) & 0xFF;
    tx_data[4] = (optical_reading >> 8) & 0xFF;
    tx_data[5] = optical_reading & 0xFF;
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
    print_hex_bytes(rx_data, 8);

    // Message to transmit
    uint8_t tx_data[8];

    // Send back the message type and field number
    tx_data[0] = rx_data[0];
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
            handle_rx_sci(tx_data);
            break;

        default:
            print("Unknown message type\n");
            break;
    }

    // TODO - should it not transmit if the received message is not recognized?

    // Enqueue TX data to transmit
    enqueue(&tx_message_queue, tx_data);
    print("Enqueued TX Message\n");
    print_hex_bytes(tx_data, 8);
}





void setup_adc(void) {
    // TODO
    // init_port_expander();
    // init_adc();
    // print("ADC Setup Completed\n");
    //
    // write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);
    // uint32_t config_data = read_ADC_register(CONFIG_ADDR);
    // print("ADC Configuration Register: %lX\n", config_data);
    //
    // int pga_gain = 1;
    // set_PGA(pga_gain);
    // print("PGA gain: %d\n", pga_gain);
}


// Initializes everything in PAY
void init_pay(void) {
    // UART
    init_uart();
    print("\n\nUART Initialized\n");

    // SPI and sensors
    init_spi();
    // TODO
    // sensor_setup();
    print("SPI and Sensors Initialized\n");

    // ADC
    setup_adc();
    print("ADC Initialized\n");

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
    print("---------------\n\n");
    print("PAY Initialized\n\n");

    // Main loop
    while (1) {
        // TODO - control system(s)?

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
