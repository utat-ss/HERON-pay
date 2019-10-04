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

void handle_hk(uint8_t field_num);
void handle_opt(uint8_t field_num);
void handle_ctrl(uint8_t field_num, uint32_t rx_data);


void handle_rx_msg(void) {
    // Get received message from queue
    uint8_t rx_msg[8] = { 0x00 };
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&rx_msg_queue)) {
            return;
        }
        dequeue(&rx_msg_queue, rx_msg);
    }

    uint8_t msg_type = rx_msg[2];
    uint8_t field_num = rx_msg[3];
    uint32_t rx_data =
        ((uint32_t) rx_msg[4] << 24) |
        ((uint32_t) rx_msg[5] << 16) |
        ((uint32_t) rx_msg[6] << 8) |
        ((uint32_t) rx_msg[7]);

    // Check message type
    switch (msg_type) {
        case CAN_PAY_HK:
            handle_hk(field_num);
            break;
        case CAN_PAY_OPT:
            handle_opt(field_num);
            break;
        case CAN_PAY_CTRL:
            handle_ctrl(field_num, rx_data);
            break;
        default:
            return;
    }
}


// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_hk(uint8_t field_num) {
    uint32_t tx_data = 0;

    // if (field_num == CAN_PAY_HK_TEMP) {
    //     if (sim_local_actions) {
    //         // 16 bit raw data, make sure the 0b11 on the right is always there
    //         tx_data = (random() & 0xFFFF) | 0b11;
    //     } else {
    //         tx_data = read_temp_raw_data();
    //     }
    // }

    if (field_num == CAN_PAY_HK_HUM) {
        tx_data = read_hum_raw_data();
    }

    else if (field_num == CAN_PAY_HK_PRES) {
        tx_data = read_pres_raw_data();
    }

    // else if ((CAN_PAY_HK_THERM0 <= field_num) &&
    //         (field_num < CAN_PAY_HK_THERM0 + 10)) {
    //     if (sim_local_actions) {
    //         // only allow 11 bits because thermistors do not exceed 2.5V
    //         // (half of the ADC's 5V range)
    //         tx_data = random() & 0x7FF;
    //     } else {
    //         uint8_t channel = field_num - CAN_PAY_HK_THERM0;
    //         fetch_adc_channel(&adc, channel);
    //         tx_data = read_adc_channel(&adc, channel);
    //     }
    // }

    // else if (field_num == CAN_PAY_HK_HEAT_SP1) {
    //     if (sim_local_actions) {
    //         tx_data = random() & 0xFFF;
    //     } else {
    //         tx_data = dac.raw_voltage_a;
    //     }
    // }

    // else if (field_num == CAN_PAY_HK_HEAT_SP2) {
    //     if (sim_local_actions) {
    //         tx_data = random() & 0xFFF;
    //     } else {
    //         tx_data = dac.raw_voltage_b;
    //     }
    // }

    else {
        // Return before calling enqueue() so we don't send a message back
        return;
    }

    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = 0x00;
    tx_msg[1] = 0x00;
    tx_msg[2] = CAN_PAY_HK;
    tx_msg[3] = field_num;
    tx_msg[4] = (tx_data >> 24) & 0xFF;
    tx_msg[5] = (tx_data >> 16) & 0xFF;
    tx_msg[6] = (tx_data >> 8) & 0xFF;
    tx_msg[7] = tx_data & 0xFF;
    // Add message to transmit
    enqueue(&tx_msg_queue, tx_msg);

    // Restart the timer for not receiving a command
    restart_cmd_timer();
}


void handle_opt(uint8_t field_num) {
    // Check the field number is valid
    if (field_num >= CAN_PAY_OPT_FIELD_COUNT) {
        return;
    }

    // Get data from PAY-Optical over SPI
    uint32_t tx_data = read_opt_spi(field_num);

    // Add a message to transmit back
    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = 0x00;
    tx_msg[1] = 0x00;
    tx_msg[2] = CAN_PAY_OPT;
    tx_msg[3] = field_num;
    tx_msg[4] = (tx_data >> 24) & 0xFF;
    tx_msg[5] = (tx_data >> 16) & 0xFF;
    tx_msg[6] = (tx_data >> 8) & 0xFF;
    tx_msg[7] = tx_data & 0xFF;
    enqueue(&tx_msg_queue, tx_msg);

    // Restart the timer for not receiving a command
    restart_cmd_timer();
}


void handle_ctrl(uint8_t field_num, uint32_t rx_data) {    
    uint32_t tx_data = 0;

    if (field_num == CAN_PAY_CTRL_PING) {
        // Don't do anything, just handle the field number so we send something back
    }

    // else if (field_num == CAN_PAY_CTRL_SET_HEAT1_SP) {
    //     set_heaters_1_to_4_raw_setpoint(rx_data);
    // }
    // else if (field_num == CAN_PAY_CTRL_SET_HEAT2_SP) {
    //     set_heater_5_raw_setpoint(rx_data);
    // }

    else if (field_num == CAN_PAY_CTRL_ACT_UP) {
        // forwards - up
        actuate_motors(40, 15, true);
    }
    else if (field_num == CAN_PAY_CTRL_ACT_DOWN) {
        // backwards - down
        actuate_motors(40, 15, false);
    }

    else if (field_num == CAN_PAY_CTRL_RESET) {
        // Note the program will stop here and restart
        reset_self_mcu(UPTIME_RESTART_REASON_RESET_CMD);
    }

    else if (field_num == CAN_PAY_CTRL_READ_EEPROM) {
        // Received rx_data as a uint32_t but need to represent it as a uint32_t*, which the eeprom function requires
        // Note that sizeof(uint32_t) = 4, sizeof(uint32_t*) = 2
        // Need to case to uint16_T first or else we get
        // warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]
        uint32_t* ptr = (uint32_t*) ((uint16_t) rx_data);
        tx_data = eeprom_read_dword(ptr);
    }

    // TODO - CAN_PAY_CTRL_ERASE_EEPROM
    // TODO - uptime/restart commands to match EPS

    else {
        return;
    }
    

    // Send back the same message type and field number
    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = 0x00;
    tx_msg[1] = 0x00;
    tx_msg[2] = CAN_PAY_CTRL;
    tx_msg[3] = field_num;
    tx_msg[4] = (tx_data >> 24) & 0xFF;
    tx_msg[5] = (tx_data >> 16) & 0xFF;
    tx_msg[6] = (tx_data >> 8) & 0xFF;
    tx_msg[7] = tx_data & 0xFF;
    // Enqueue TX message to transmit
    enqueue(&tx_msg_queue, tx_msg);

    // Restart the timer for not receiving a command
    restart_cmd_timer();
}
