/*
Defines the logical interface that PAY uses for CAN communication. This is for
handling received CAN messages, performing actions, and responding.

Authors: Bruno Almeida
*/

#include "can_commands.h"


/* Message queues */

// CAN messages received but not processed yet
queue_t rx_msg_queue;
// CAN messages to transmit
queue_t tx_msg_queue;

void handle_hk(uint8_t* rx_data);
void handle_sci(uint8_t* rx_data);
void handle_motor(uint8_t* rx_data);


void handle_rx_msg(void) {
    if (queue_empty(&rx_msg_queue)) {
        print("RX queue empty\n");
        return;
    }

    // Received message
    uint8_t rx_data[8] = { 0x00 };
    dequeue(&rx_msg_queue, rx_data);

    // Check message type
    switch (rx_data[1]) {
        case CAN_PAY_HK:
            handle_hk(rx_data);
            break;
        case CAN_PAY_SCI:
            handle_sci(rx_data);
            return;
        case CAN_PAY_MOTOR:
            handle_motor(rx_data);
            break;
        default:
            break;
    }
}


// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_hk(uint8_t* rx_data) {
    uint8_t tx_data[8] = { 0x00 };
    tx_data[0] = 0; // TODO
    tx_data[1] = rx_data[1];
    tx_data[2] = rx_data[2];

    // Declare these here because we can't start a case statement with a declaration
    uint16_t raw_temp_data;
    uint16_t raw_hum_data;
    uint32_t raw_pres_data;

    // Check field number
    switch (rx_data[2]) {
        case CAN_PAY_HK_TEMP:
            raw_temp_data = temp_read_raw_data();
            tx_data[3] = 0x00;
            tx_data[4] = (raw_temp_data >> 8) & 0xFF;
            tx_data[5] = raw_temp_data & 0xFF;
            break;

        case CAN_PAY_HK_HUMID:
            raw_hum_data = hum_read_raw_data();
            tx_data[3] = 0x00;
            tx_data[4] = (raw_hum_data >> 8) & 0xFF;
            tx_data[5] = raw_hum_data & 0xFF;
            break;

        case CAN_PAY_HK_PRES:
            raw_pres_data = pres_read_raw_data();
            tx_data[3] = (raw_pres_data >> 16) & 0xFF;
            tx_data[4] = (raw_pres_data >> 8) & 0xFF;
            tx_data[5] = raw_pres_data & 0xFF;
            break;

        default:
            return; // don't send a message back
    }

    // Enqueue TX data to transmit
    enqueue(&tx_msg_queue, tx_data);
}


void handle_sci(uint8_t* rx_data) {
    // TODO - higher level optical spi function that delays until interrupts
    // send_read_sensor_command(rx_data[2]);

    // Random data for now
    uint32_t raw_optical = rand() % 32767;

    uint8_t tx_data[8] = { 0x00 };
    tx_data[0] = 0; // TODO
    tx_data[1] = rx_data[1];
    tx_data[2] = rx_data[2];
    tx_data[3] = (raw_optical >> 16) & 0xFF;
    tx_data[4] = (raw_optical >> 8) & 0xFF;
    tx_data[5] = raw_optical & 0xFF;

    enqueue(&tx_msg_queue, tx_data);
}


void handle_motor(uint8_t* rx_data) {
    if (rx_data[2] == CAN_PAY_MOTOR_ACTUATE) {
        // TODO
        // actuate_motors();

        // Send back the same message type and field number
        uint8_t tx_data[8] = { 0x00 };
        tx_data[0] = 0; // TODO
        tx_data[1] = rx_data[1];
        tx_data[2] = rx_data[2];

        // Enqueue TX data to transmit
        enqueue(&tx_msg_queue, tx_data);
    }
}
