#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/packets.h>
#include <stdbool.h>

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
            break;
    }
}

void tx_callback_sci(uint8_t* data, uint8_t *len) {
    // Last 6 bits
    switch (rx_data[1] & 0b111111) {
        case 0:
            print("Optical 0\n");
            break;

        case 1:
            print("Optical 1\n");
            break;
    }
}

void tx_callback(uint8_t* data, uint8_t* len) {
    print("TX callback\n");

    *len = 8;

    data[0] = rx_data[0];
    data[1] = rx_data[1];

    // TODO remove
    data[2] = 0x09;

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
