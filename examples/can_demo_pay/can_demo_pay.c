#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/packets.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../../src/sensors.h"

void rx_callback(uint8_t*, uint8_t);
void tx_callback(uint8_t*, uint8_t*);

mob_t rx_mob = {
    .mob_num = 1,
    .mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0001 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

mob_t tx_mob = {
    .mob_num = 2,
    .mob_type = TX_MOB,
    .id_tag = { 0x0001 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

uint8_t rx_data[8];
volatile bool respond = false;
uint32_t response_counter = 0;


void print_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
}


void rx_callback(uint8_t* data, uint8_t len) {
    print("\nRX callback!\n");
    // print("%s\n", (char *) data);

    print("Received Data:\n");
    print_bytes(data, len);

    // Store RX data
    for (int i = 0; i < 8; i++) {
        rx_data[i] = data[i];
    }

    respond = true;
}


void tx_callback_hk(uint8_t* data, uint8_t *len) {
    switch (rx_data[1]) {
        case PAY_TEMP_1:
            print("PAY_TEMP_1\n");
            break;

        case PAY_PRES_1:
            print("PAY_PRES_1\n");
            break;

        case PAY_HUMID_1:
            print("PAY_HUMID_1\n");

            print("GETTING HUMIDITY\n");
            uint32_t raw_humidity = read_raw_humidity();
            print("DONE GETTING HUMIDITY\n");
            data[2] = 0x00;
            data[3] = (raw_humidity >> 24) & 0xFF;
            data[4] = (raw_humidity >> 16) & 0xFF;

            break;
    }
}

void tx_callback_sci(uint8_t* data, uint8_t *len) {
	uint8_t well_num = rx_data[1] & 0x3F;

    int channel = 0;
    int LED = 0;

	// Check 6 bytes for well number, poll appropriate sensor
	// TODO - add more sensors
	switch (well_num) {
        // TEMD
        case 0:
            print("Optical 0\n");
        	channel = 5;
        	LED = LED_2;
            break;

        // SFH
        case 1:
            print("Optical 1\n");
        	channel = 6;
        	LED = LED_3;
            break;
	}

    // TODO - activate LED

    print("Well %d, Channel %d, LED %d\n", well_num, channel, LED);

    // Get random dummy value
    // uint32_t reading = read_ADC_channel(channel);
    uint32_t reading = rand() % ((uint32_t) 1 << 16);  // dummy value
    reading |= (rand() % ((uint32_t) 1 << 8)) << 16;
	// print("0x%02x%02x%02x\n", (reading >> 16) & 0xFF, (reading >> 8) & 0xFF, reading & 0xFF);

	data[2] = (reading >> 16) & 0xFF;
	data[3] = (reading >> 8) & 0xFF;
	data[4] = reading & 0xFF;
}

void tx_callback(uint8_t* data, uint8_t* len) {
    print("TX callback\n");

    *len = 8;

    data[0] = rx_data[0];
    data[1] = rx_data[1];

    // TODO remove
    // data[2] = 0x09;
    data[2] = (response_counter >> 16) & 0xFF;
    data[3] = (response_counter >> 8) & 0xFF;
    data[4] = (response_counter >> 0) & 0xFF;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;

    response_counter += ((uint32_t) 1 << 16) - 1;

    switch (rx_data[0]) {
        case PAY_HK_REQ:
            print("PAY_HK_REQ\n");
            tx_callback_hk(data, len);
            break;

        case PAY_SCI_REQ:
            print("PAY_SCI_REQ\n");
            tx_callback_sci(data, len);
            break;
    }

    print("Transmitting Data:\n");
    print_bytes(data, *len);

    respond = false;
}


int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    sensor_setup();

    init_can();
    init_rx_mob(&rx_mob);
    init_tx_mob(&tx_mob);



    print("Waiting for RX\n");
    while (1) {
        if (respond) {
            print("Before Resume\n");
            resume_mob(&tx_mob);
            print("After Resume\n");
        }
    }
}
