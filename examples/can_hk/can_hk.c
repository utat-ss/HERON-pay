#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/packets.h>
#include <can/can_ids.h>

#include <util/delay.h>
#include <stdint.h>

void tx_callback(uint8_t*, uint8_t*);
void rx_callback(uint8_t*, uint8_t);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = PAY_CMD_RX_MOB_ID,
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

mob_t rx_mob = {
    .mob_num = 1,
    .mob_type = RX_MOB,
    .dlc = 7,
    .id_tag = { 0x0001 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

void tx_callback(uint8_t* data, uint8_t* len) {
    print("-------------------------------\n");
    print("tx_callback()\n");

    uint8_t req[8] = { PAY_HK_REQ, PAY_TEMP_1 };
    print("Sending packet: ");

    *len = 8;
    for (int i = 0; i < *len; i++) {
        data[i] = req[i];
        print("%x ", data[i]);
    }

    print("\n");
    print("-------------------------------\n");
}

void rx_callback(uint8_t* data, uint8_t len) {
    print("-------------------------------\n");
    print("rx_callback()\n");
    print("Packet received: \n");
    print("Packet type 0x%02x\n", data[0]); // should be HK_REQ
    print("Packet field 0x%02x\n", data[1]); // should be TEMP
    print("Packet contents:\n");
    for (int i = 2; i < len; i++) {
        print("0x%02x\n", data[i]);
    }
    print("-------------------------------\n");
}

int main(void) {
    init_uart();
    init_can();

    init_tx_mob(&tx_mob);
    init_rx_mob(&rx_mob);

    while (1) {
        resume_mob(&tx_mob);
    }

    return 0;
}
