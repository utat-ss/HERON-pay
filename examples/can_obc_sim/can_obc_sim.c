#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/can_ids.h>
#include <can/packets.h>
#include <util/delay.h>
#include "../../src/sensors.h"

int req_num = 0;    // 0-4 -> 0-1

void tx_callback(uint8_t*, uint8_t*);
void rx_callback(uint8_t*, uint8_t);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    // .id_tag = { 0x0000 },
    .id_tag = OBC_PAY_CMD_TX_MOB_ID,
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

mob_t rx_mob = {
    .mob_num = 3,
    .mob_type = RX_MOB,
    .dlc = 8,
    // .id_tag = { 0x0000 },
    // .id_mask = { 0x0000 },
    .id_tag = OBC_DATA_RX_MOB_ID,
    .id_mask = CAN_RX_MASK_ID,
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

void print_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
}

void tx_callback(uint8_t* data, uint8_t* len) {
    print("\nTX Callback\n");

    *len = 8;

    switch (req_num) {
        // case 0:
        //     data[0] = PAY_HK_REQ;
        //     data[1] = PAY_TEMP_1;
        //     break;
        //
        // case 1:
        //     data[0] = PAY_HK_REQ;
        //     data[1] = PAY_PRES_1;
        //     break;
        //
        // case 2:
        //     data[0] = PAY_HK_REQ;
        //     data[1] = PAY_HUMID_1;
        //     break;
        //
        // case 3:
        //     data[0] = PAY_SCI_REQ;
        //     data[1] = 0b0;
        //     break;
        //
        // case 4:
        //     data[0] = PAY_SCI_REQ;
        //     data[1] = 0b1;
        //     break;

        case 0:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_TEMP_1;
            break;

        case 1:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_HUMID_1;
            break;

        default:
            req_num = 0;
            break;
    }

    req_num++;
    if (req_num > 1) {
        req_num = 0;
    }

    print("Transmitting Data:\n");
    print_bytes(data, *len);
}


void rx_callback(uint8_t* data, uint8_t len) {
    print("RX callback!\n");

    print("Received Data:\n");
    print_bytes(data, len);

    if (data[0] == PAY_HK_REQ) {
        uint16_t raw_temperature = 0;
        uint32_t raw_humidity = 0;
        double converted;

        switch (data[1]) {
            case PAY_TEMP_1:
                raw_temperature = ((uint16_t)data[3] << 8) | (uint16_t)data[4];
                converted = convert_temperature(raw_temperature);
                print("--------\n");
                print("Temperature: %d C\n",(int)converted);
                print("--------\n");
                break;

            case PAY_HUMID_1:
                raw_humidity = ((uint32_t)data[3] << 24) | ((uint32_t)data[4] << 16);
                converted = convert_humidity(raw_humidity);
                print("--------\n");
                print("Humidity: %d percent\n",(int)converted);
                print("--------\n");
                break;

            default:
                break;
        }
    }
}


int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    init_can();
    init_tx_mob(&tx_mob);
    init_rx_mob(&rx_mob);

    while(1) {
        resume_mob(&tx_mob);
        while (!is_paused(&tx_mob)) {};
        _delay_ms(2000);
    }

    print("Status: %#02x\n", mob_status(&tx_mob));
    print("Tx error count: %d\n", CANTEC);

}