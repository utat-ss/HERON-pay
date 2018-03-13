#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/packets.h>
#include <util/delay.h>

int req_num = 0;    // 0-4

void tx_callback(uint8_t*, uint8_t*);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = { 0x0000 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

void print_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
}

void tx_callback(uint8_t* data, uint8_t* len) {
    // *len = 8;
    // char str[] = "Working";
    //
    // for(uint8_t i = 0; i < *len; i++) {
    //     data[i] = str[i];
    // }

    *len = 8;

    switch (req_num) {
        case 0:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_TEMP_1;
            break;

        case 1:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_PRES_1;
            break;

        case 2:
            data[0] = PAY_HK_REQ;
            data[1] = PAY_HUMID_1;
            break;

        case 3:
            data[0] = PAY_SCI_REQ;
            data[1] = 0b0;
            break;

        case 4:
            data[0] = PAY_SCI_REQ;
            data[1] = 0b1;
            break;

        default:
            req_num = 0;
            break;
    }

    req_num++;
    if (req_num > 4) {
        req_num = 0;
    }

    print("Transmitting Data:\n");
    print_bytes(data, *len);
}

int main(void) {
    init_uart();
    print("UART initialized\n");

    init_can();
    init_tx_mob(&tx_mob);

    while (1) {
        resume_mob(&tx_mob);
        while (!is_paused(&tx_mob)) {};
        _delay_ms(2000);

        print("Status: %#02x\n", mob_status(&tx_mob));
        print("Tx error count: %d\n", CANTEC);
    }
}
