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

// Set this to true to simulate performing all local actions (e.g. fetching
// data, actuating motors) - this allows testing just the PAY command handling
// system on any PCB without any peripherals
bool sim_local_actions = false;

void handle_hk(uint8_t* rx_msg);
void handle_opt(uint8_t* rx_msg);
void handle_ctrl(uint8_t* rx_msg);


void handle_rx_msg(void) {
    // Get received message from queue
    uint8_t rx_msg[8] = { 0x00 };
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&rx_msg_queue)) {
            return;
        }
        dequeue(&rx_msg_queue, rx_msg);
    }

    // Check message type
    switch (rx_msg[1]) {
        case CAN_PAY_HK:
            handle_hk(rx_msg);
            break;
        case CAN_PAY_OPT:
            handle_opt(rx_msg);
            break;
        case CAN_PAY_CTRL:
            handle_ctrl(rx_msg);
            break;
        default:
            return;
    }
}


// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_hk(uint8_t* rx_msg) {
    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = 0; // TODO
    tx_msg[1] = rx_msg[1];
    tx_msg[2] = rx_msg[2];

    // Check field number
    uint8_t field_num = rx_msg[2];
    uint32_t raw_data = 0;

    if (field_num == CAN_PAY_HK_TEMP) {
        if (sim_local_actions) {
            // 16 bit raw data, make sure the 0b11 on the right is always there
            raw_data = (random() & 0xFFFF) | 0b11;
        } else {
            raw_data = read_temp_raw_data();
        }
    }

    else if (field_num == CAN_PAY_HK_HUM) {
        if (sim_local_actions) {
            // 14 bit raw data
            raw_data = random() & 0x3FFF;
        } else {
            raw_data = read_hum_raw_data();
        }
    }

    else if (field_num == CAN_PAY_HK_PRES) {
        if (sim_local_actions) {
            raw_data = random() & 0xFFFFFF;
        } else {
            raw_data = read_pres_raw_data();
        }
    }

    else if ((CAN_PAY_HK_THERM0 <= field_num) &&
            (field_num < CAN_PAY_HK_THERM0 + 10)) {
        if (sim_local_actions) {
            // only allow 11 bits because thermistors do not exceed 2.5V
            // (half of the ADC's 5V range)
            raw_data = random() & 0x7FF;
        } else {
            uint8_t channel = field_num - CAN_PAY_HK_THERM0;
            fetch_adc_channel(&adc, channel);
            raw_data = read_adc_channel(&adc, channel);
        }
    }

    else if (field_num == CAN_PAY_HK_HEAT_SP1) {
        if (sim_local_actions) {
            raw_data = random() & 0xFFF;
        } else {
            raw_data = dac.raw_voltage_a;
        }
    }

    else if (field_num == CAN_PAY_HK_HEAT_SP2) {
        if (sim_local_actions) {
            raw_data = random() & 0xFFF;
        } else {
            raw_data = dac.raw_voltage_b;
        }
    }

    else if (field_num == CAN_PAY_HK_PROX_LEFT) {
        if (sim_local_actions) {
            raw_data = random() & 0xFFF;
        } else {
            uint8_t channel = 10;
            fetch_adc_channel(&adc, channel);
            raw_data = read_adc_channel(&adc, channel);
        }
    }

    else if (field_num == CAN_PAY_HK_PROX_RIGHT) {
        if (sim_local_actions) {
            raw_data = random() & 0xFFF;
        } else {
            uint8_t channel = 11;
            fetch_adc_channel(&adc, channel);
            raw_data = read_adc_channel(&adc, channel);
        }
    }

    else {
        // Return before calling enqueue() so we don't send a message back
        return;
    }

    tx_msg[3] = (raw_data >> 16) & 0xFF;
    tx_msg[4] = (raw_data >> 8) & 0xFF;
    tx_msg[5] = raw_data & 0xFF;

    // Add message to transmit
    enqueue(&tx_msg_queue, tx_msg);
}


void handle_opt(uint8_t* rx_msg) {
    // Check the field number is valid
    uint8_t field_num = rx_msg[2];
    if (field_num >= CAN_PAY_OPT_FIELD_COUNT) {
        return;
    }

    uint32_t raw_optical = 0;
    if (sim_local_actions) {
        // 24 bit raw data
        raw_optical = random() & 0xFFFFFF;
    } else {
        // Get data from PAY-Optical over SPI
        raw_optical = read_opt_spi(field_num);
    }

    // Add a message to transmit back
    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = 0; // TODO
    tx_msg[1] = rx_msg[1];
    tx_msg[2] = rx_msg[2];
    tx_msg[3] = (raw_optical >> 16) & 0xFF;
    tx_msg[4] = (raw_optical >> 8) & 0xFF;
    tx_msg[5] = raw_optical & 0xFF;
    enqueue(&tx_msg_queue, tx_msg);
}


void handle_ctrl(uint8_t* rx_msg) {
    // Send back the same message type and field number
    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = 0; // TODO
    tx_msg[1] = rx_msg[1];
    tx_msg[2] = rx_msg[2];

    uint32_t raw_data =
        (((uint32_t) rx_msg[3]) << 16) |
        (((uint32_t) rx_msg[4]) << 8) |
        (((uint32_t) rx_msg[5]));

    switch (rx_msg[2]) {
        case CAN_PAY_CTRL_HEAT_SP1:
            if (!sim_local_actions) {
                set_heaters_1_to_4_raw_setpoint(raw_data);
            }
            break;
        case CAN_PAY_CTRL_HEAT_SP2:
            if (!sim_local_actions) {
                set_heater_5_raw_setpoint(raw_data);
            }
            break;
        case CAN_PAY_CTRL_ACT_UP:
            if (!sim_local_actions) {
                // forwards - up
                actuate_motors(40, 15, true);
            }
            break;

        case CAN_PAY_CTRL_ACT_DOWN:
            if (!sim_local_actions) {
                // backwards - down
                actuate_motors(40, 15, false);
            }
            break;

        default:
            return;
    }

    tx_msg[3] = (raw_data >> 16) & 0xFF;
    tx_msg[4] = (raw_data >> 8) & 0xFF;
    tx_msg[5] = raw_data & 0xFF;

    // Enqueue TX message to transmit
    enqueue(&tx_msg_queue, tx_msg);
}
