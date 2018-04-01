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

// Counter for which request to send (0-3)
uint8_t req_num = 0;
uint8_t num_req_nums = 4;

void tx_callback(uint8_t*, uint8_t*);
void rx_callback(const uint8_t*, uint8_t);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = OBC_PAY_CMD_TX_MOB_ID,
    .ctrl = default_tx_ctrl,

    .tx_data_cb = tx_callback
};

mob_t rx_mob = {
    .mob_num = 3,
    .mob_type = RX_MOB,
    .dlc = 8,
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
        case 0:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_TEMP_1;
            break;

        case 1:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_HUMID_1;
            break;

        case 2:
            data[0] = PAY_SCI_REQ;
            data[1] = PAY_SCI_TEMD;
            break;

        case 3:
            data[0] = PAY_SCI_REQ;
            data[1] = PAY_SCI_SFH;
            break;

        default:
            req_num = 0;
            break;
    }

    req_num = (req_num + 1) % num_req_nums;

    print("Transmitting Message:\n");
    print_bytes(data, *len);
}


void rx_callback(const uint8_t* data, uint8_t len) {
    print("RX Callback\n");
    print("Received Message:\n");
    print_bytes((uint8_t *) data, len);

    if (data[0] == PAY_HK_REQ) {
        uint16_t raw_temperature = 0;
        uint32_t raw_humidity = 0;
        double converted;

        switch (data[1]) {
            case PAY_TEMP_1:
                raw_temperature = ((uint16_t) data[3] << 8) | (uint16_t) data[4];
                converted = convert_temperature(raw_temperature);
                print("--------\n");
                print("Temperature: %d C\n",(int)converted);
                print("--------\n");

                break;

            case PAY_HUMID_1:
                raw_humidity = ((uint32_t) data[3] << 24) | ((uint32_t) data[4] << 16);
                converted = convert_humidity(raw_humidity);
                print("--------\n");
                print("Humidity: %d percent\n",(int)converted);
                print("--------\n");

                break;

            default:
                break;
        }
    }

    if (data[0] == PAY_SCI_REQ) {
        switch (data[1]) {
            case PAY_SCI_TEMD:
                print("TEMD\n");
                break;

            case PAY_SCI_SFH:
                print("SFH\n");
                break;

            default:
                break;
        }

        uint32_t raw_optical =
                ((uint32_t) data[2] << 16) |
                ((uint32_t) data[3] << 8 ) |
                ((uint32_t) data[4]      );
        uint32_t raw_optical_max = 0xFFFFFF;
		print("%lu / %lu\n", raw_optical, raw_optical_max);

        double fraction = ((double) raw_optical / (double) raw_optical_max);
        print("--------\n");
        print("Optical: %d percent\n", (uint8_t) (fraction * 100.0));
        print("--------\n");
    }
}




int main(void) {
    init_uart();
    print("\n\nUART Initialized\n");

    init_can();
    init_tx_mob(&tx_mob);
    init_rx_mob(&rx_mob);
    print("CAN Initialized\n");

    while(1) {
        resume_mob(&tx_mob);
        while (!is_paused(&tx_mob)) {};
        _delay_ms(2000);
    }
}
