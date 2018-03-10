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


// TODO - might need to modify these MOBs to match the OBC MOBs
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

uint8_t sensor_to_poll = 0;

void tx_callback(uint8_t* data, uint8_t* len) {
    if (sensor_to_poll == 0) {
        print("-------------------------------\n");
        print("tx_callback()\n");

        uint8_t req[8] = { PAY_HK_REQ, PAY_TEMP_1 };
        *len = 8;

        print("Transmitting CAN Message: ");
        for (int i = 0; i < *len; i++) {
            data[i] = req[i];
            print("0x%02x ", data[i]);
        }
        print("\n");

        print("Packet: 0x%02x\n", data[0]); // should be HK_REQ
        print("Field: 0x%02x\n", data[1]); // should be TEMP_1

        print("\n");
        print("-------------------------------\n");
    }

    else {
        print("-------------------------------\n");
        print("tx_callback()\n");
        print("Sensor to poll: %d\n", sensor_to_poll);

        *len = 8;

        switch (sensor_to_poll) {
            case 1:
                data[0] = PAY_HK_REQ;
                data[1] = PAY_TEMP_1;
                break;
            case 2:
                data[0] = PAY_HK_REQ;
                data[1] = PAY_PRES_1;
                break;
            case 3:
                data[0] = PAY_HK_REQ;
                data[1] = PAY_HUMID_1;
                break;
            case 4:
                data[0] = PAY_HK_REQ;
                data[1] = PAY_MF_TEMP_1;
                break;
            case 5:
                data[0] = PAY_HK_REQ;
                data[1] = PAY_MF_TEMP_2;
                break;
            case 6:
                data[0] = PAY_HK_REQ;
                data[1] = PAY_MF_TEMP_3;
                break;
            case 7:
                data[0] = PAY_SCI_REQ;
                data[1] = 0b0;
                break;
            case 8:
                data[0] = PAY_SCI_REQ;
                data[1] = 0b1;
                break;
            default:
                break;
        }

        print("Transmitting CAN Message: ");
        for (int i = 0; i < *len; i++) {
            print("0x%02x ", data[i]);
        }
        print("\n");

        print("Packet: 0x%02x\n", data[0]);
        print("Field: 0x%02x\n", data[1]);

        print("\n");
        print("-------------------------------\n");
    }
}

void rx_callback(uint8_t* data, uint8_t len) {
    print("-------------------------------\n");
    print("rx_callback()\n");

    print("Received CAN Message: ");
    for (int i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");

    print("Packet: 0x%02x\n", data[0]); // should be HK_REQ
    print("Field: 0x%02x\n", data[1]); // should be TEMP_1
    print("Data: ");
    for (int i = 2; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
    print("-------------------------------\n");
}

void print_sensor_list() {
    print("\nPoll Sensor:\n");
    print("1. Temperature 1\n");
    print("2. Pressure 1\n");
    print("3. Humidity 1\n");
    print("4. MF Temperature 1\n");
    print("5. MF Temperature 2\n");
    print("6. MF Temperature 3\n");
    print("7. Optical 1 (TEMD)\n");
    print("8. Optical 2 (SFH)\n");
}

void uart_read_callback(uint8_t *buf, uint8_t len) {
    print("uart_read_callback()\n");

    for (int i = 0; i < len; i++) {
        print("UART: Received %c\n", buf[i]);
        sensor_to_poll = (int) (buf[i] - '0');
    }
    resume_mob(&tx_mob);

    clear_rx_buffer();
    print_sensor_list();
}


int main(void) {
    init_uart();
    init_can();

    init_tx_mob(&tx_mob);
    init_rx_mob(&rx_mob);

    while (1) {
        resume_mob(&tx_mob);
        _delay_ms(10000);
    }

    register_callback(uart_read_callback);
    print_sensor_list();

    return 0;
}
