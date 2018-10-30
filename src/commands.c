/*
Commands initiated by received CAN messages.

Authors: Bruno Almeida
*/

#include "commands.h"

void handle_hk(uint8_t* rx_data);
void handle_sci(uint8_t* rx_data);
void handle_motor(uint8_t* rx_data);


void handle_rx_msg(void) {
    if (queue_empty(&can_rx_msgs)) {
        print("RX queue empty\n");
        return;
    }

    // Received message
    uint8_t rx_data[8];
    dequeue(&can_rx_msgs, rx_data);
    print("Dequeued RX\n");
    print_bytes(rx_data, 8);

    // Check message type
    switch (rx_data[1]) {
        case CAN_PAY_HK:
            print("PAY_HK\n");
            handle_hk(rx_data);
            break;

        case CAN_PAY_SCI:
            print("PAY_SCI\n");
            handle_sci(rx_data);
            return;

        case CAN_PAY_MOTOR:
            print("PAY_MOTOR\n");
            handle_motor(rx_data);
            break;

        default:
            print("Unknown message type\n");
            break;
    }
}


// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_hk(uint8_t* rx_data) {
    uint8_t tx_data[8] = { 0 };
    tx_data[0] = 0; // TODO
    tx_data[1] = rx_data[1];
    tx_data[2] = rx_data[2];

    // Check field number
    switch (rx_data[2]) {
        case CAN_PAY_HK_TEMP:
            print("PAY_HK_TEMP\n");

            uint16_t raw_temp_data = temp_read_raw_data();

            tx_data[3] = 0x00;
            tx_data[4] = (raw_temp_data >> 8) & 0xFF;
            tx_data[5] = raw_temp_data & 0xFF;

            break;

        case CAN_PAY_HK_HUMID:
            print("PAY_HK_HUMID\n");

            uint16_t raw_hum_data = hum_read_raw_data();

            tx_data[3] = 0x00;
            tx_data[4] = (raw_hum_data >> 8) & 0xFF;
            tx_data[5] = raw_hum_data & 0xFF;

            break;

        case CAN_PAY_HK_PRES:
            print("PAY_HK_PRES\n");

            uint32_t raw_pres_data = pres_read_raw_data();

            tx_data[3] = (raw_pres_data >> 16) & 0xFF;
            tx_data[4] = (raw_pres_data >> 8) & 0xFF;
            tx_data[5] = raw_pres_data & 0xFF;

            break;

        default:
            print("Unknown field number\n");
            return; // don't send a message back
    }

    // Enqueue TX data to transmit
    enqueue(&can_tx_msgs, tx_data);
    print("Enqueued TX\n");
    print_bytes(tx_data, 8);
}


void handle_sci(uint8_t* rx_data) {
    // TODO - higher level optical spi function that delays until interrupts
    // send_read_sensor_command(rx_data[2]);

    // Random data for now
    uint32_t raw_optical = rand() % 32767;

    uint8_t tx_data[8];
    tx_data[0] = 0; // TODO
    tx_data[1] = rx_data[1];
    tx_data[2] = rx_data[2];
    tx_data[3] = (raw_optical >> 16) & 0xFF;
    tx_data[4] = (raw_optical >> 8) & 0xFF;
    tx_data[5] = raw_optical & 0xFF;

    enqueue(&can_tx_msgs, tx_data);
    print("Enqueued TX\n");
    print_bytes(tx_data, 8);
}


void handle_motor(uint8_t* rx_data) {
    if (rx_data[2] == CAN_PAY_MOTOR_ACTUATE) {
        // TODO
        // actuate_motors();

        // Send back the same message type and field number
        uint8_t tx_data[8];
        tx_data[0] = 0; // TODO
        tx_data[1] = rx_data[1];
        tx_data[2] = rx_data[2];

        // Enqueue TX data to transmit
        enqueue(&can_tx_msgs, tx_data);
        print("Enqueued TX\n");
        print_bytes(tx_data, 8);
    }
}
