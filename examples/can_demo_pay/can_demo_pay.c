#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/packets.h>

void rx_callback(uint8_t*, uint8_t);

mob_t rx_mob = {
    .mob_num = 0,
    .mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

void print_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
}

void rx_callback_hk(uint8_t* data, uint8_t len) {
    switch (data[1]) {
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

void rx_callback_sci(uint8_t* data, uint8_t len) {
    // Last 6 bits
    switch (data[1] & 0b111111) {
        case 0:
            print("Optical 0\n");
            break;

        case 1:
            print("Optical 1\n");
            break;
    }
}

void rx_callback(uint8_t* data, uint8_t len) {
    print("\nRX received!\n");
    // print("%s\n", (char *) data);

    print("Received Data:\n");
    print_bytes(data, len);

    switch (data[0]) {
        case PAY_HK_REQ:
            print("PAY_HK_REQ\n");
            rx_callback_hk(data, len);
            break;

        case PAY_SCI_REQ:
            print("PAY_SCI_REQ\n");
            rx_callback_sci(data, len);
            break;
    }
}

int main(void) {
    init_uart();
    print("UART initialized\n");

    init_can();
    init_rx_mob(&rx_mob);

    print("Waiting for TX\n");
    while (1) {}
}
