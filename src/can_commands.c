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

    uint8_t opcode = rx_msg[2];
    uint8_t field_num = rx_msg[3];
    uint32_t rx_data =
        ((uint32_t) rx_msg[4] << 24) |
        ((uint32_t) rx_msg[5] << 16) |
        ((uint32_t) rx_msg[6] << 8) |
        ((uint32_t) rx_msg[7]);

    // Check message type
    switch (opcode) {
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

    // TODO - fill in field implementations

    if (field_num == CAN_PAY_HK_HUM) {
        tx_data = read_hum_raw_data();
    }

    else if (field_num == CAN_PAY_HK_PRES) {
        tx_data = read_pres_raw_data();
    }

    else if (field_num == CAN_PAY_HK_AMB_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MOT1_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MOT2_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_10V_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_6V_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF1_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF2_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF3_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF4_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF5_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF6_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF7_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF8_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF9_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF10_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF11_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_MF12_TEMP) {
    }

    else if (field_num == CAN_PAY_HK_HEAT_EN) {
    }

    else if (field_num == CAN_PAY_HK_LIM_PRESS) {
    }

    else if (field_num == CAN_PAY_HK_UPTIME) {
        tx_data = uptime_s;
    }

    else if (field_num == CAN_PAY_HK_RESTART_COUNT) {
        tx_data = restart_count;
    }

    else if (field_num == CAN_PAY_HK_RESTART_REASON) {
        tx_data = restart_reason;
    }

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
    restart_com_timeout();
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
    restart_com_timeout();
}


void handle_ctrl(uint8_t field_num, uint32_t rx_data) {    
    uint32_t tx_data = 0;

    // TODO - fill in field implementations

    if (field_num == CAN_PAY_CTRL_PING) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT1_OFF) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT1_ON) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT2_OFF) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT2_ON) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT3_OFF) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT3_ON) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT4_OFF) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT4_ON) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT5_OFF) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_HEAT5_ON) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_DISABLE_6V) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_ENABLE_6V) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_DISABLE_10V) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_ENABLE_10V) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_ACT_UP) {
        // forwards - up
        actuate_motors(40, 15, true);
    }

    else if (field_num == CAN_PAY_CTRL_ACT_DOWN) {
        // backwards - down
        actuate_motors(40, 15, false);
    }

    else if (field_num == CAN_PAY_CTRL_BLIST_DEP_SEQ) {
    }

    else if (field_num == CAN_PAY_CTRL_RESET) {
        reset_self_mcu(UPTIME_RESTART_REASON_RESET_CMD);
        // Note the program will stop here and restart
    }

    else if (field_num == CAN_PAY_CTRL_READ_EEPROM) {
        tx_data = read_eeprom((uint16_t) rx_data);
    }

    else if (field_num == CAN_PAY_CTRL_ERASE_EEPROM) {
        write_eeprom((uint16_t) rx_data, EEPROM_DEF_DWORD);
    }

    else if (field_num == CAN_PAY_CTRL_READ_RAM_BYTE) {
        // See lib-common/examples/read_registers for an MMIO example
        // https://arduino.stackexchange.com/questions/56304/how-do-i-directly-access-a-memory-mapped-register-of-avr-with-c
        // http://download.mikroe.com/documents/compilers/mikroc/avr/help/avr_memory_organization.htm

        // Need to represent address as volatile uint8_t* to read RAM
        // Must first cast to uint16_t or else we get warning: cast to pointer
        // from integer of different size -Wint-to-pointer-cast]
        volatile uint8_t* pointer = (volatile uint8_t*) ((uint16_t) rx_data);
        tx_data = (uint32_t) (*pointer);
    }

    else if (field_num == CAN_PAY_CTRL_START_TEMP_LPM) {
    }

    else if (field_num == CAN_PAY_CTRL_ENABLE_INDEF_LPM) {
    }

    else if (field_num == CAN_PAY_CTRL_DISABLE_INDEF_LPM) {
    }

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
    restart_com_timeout();
}
