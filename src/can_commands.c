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

// Set to true to print TX and RX CAN messages
bool print_can_msgs = true;


void handle_hk(uint8_t field_num, uint8_t* tx_status, uint32_t* tx_data);
void handle_opt(uint8_t field_num, uint8_t* tx_status);
void handle_ctrl(uint8_t field_num, uint32_t rx_data, uint8_t* tx_status,
        uint32_t* tx_data);


void process_next_rx_msg(void) {
    // Get received message from queue
    uint8_t rx_msg[8] = { 0x00 };
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&rx_msg_queue)) {
            return;
        }
        dequeue(&rx_msg_queue, rx_msg);
    }

    if (print_can_msgs) {
        // Extra spaces to align with CAN TX messages
        print("CAN RX: ");
        print_bytes(rx_msg, 8);
    }

    uint8_t opcode = rx_msg[0];
    uint8_t field_num = rx_msg[1];
    uint32_t rx_data =
        ((uint32_t) rx_msg[4] << 24) |
        ((uint32_t) rx_msg[5] << 16) |
        ((uint32_t) rx_msg[6] << 8) |
        ((uint32_t) rx_msg[7]);
    
    // By default assume success
    uint8_t tx_status = CAN_STATUS_OK;
    uint32_t tx_data = 0;

    // Check message type
    switch (opcode) {
        case CAN_PAY_HK:
            handle_hk(field_num, &tx_status, &tx_data);
            break;
        case CAN_PAY_OPT:
            handle_opt(field_num, &tx_status);
            // TODO - return here to not sent a TX message if tx_status is OK?
            break;
        case CAN_PAY_CTRL:
            handle_ctrl(field_num, rx_data, &tx_status, &tx_data);
            break;
        default:
            tx_status = CAN_STATUS_INVALID_OPCODE;
            break;
    }

    // TODO - only do this if PAY-Optical read is not in progress

    uint8_t tx_msg[8] = { 0x00 };
    tx_msg[0] = opcode;
    tx_msg[1] = field_num;
    tx_msg[2] = tx_status;
    tx_msg[3] = 0x00;
    tx_msg[4] = (tx_data >> 24) & 0xFF;
    tx_msg[5] = (tx_data >> 16) & 0xFF;
    tx_msg[6] = (tx_data >> 8) & 0xFF;
    tx_msg[7] = tx_data & 0xFF;
    // Add message to transmit
    enqueue(&tx_msg_queue, tx_msg);

    // Restart the timer for not receiving a command
    restart_com_timeout();
}


// Assuming a housekeeping request was received,
// retrieves and places the appropriate data in the tx_data buffer
void handle_hk(uint8_t field_num, uint8_t* tx_status, uint32_t* tx_data) {
    // TODO - fill in field implementations

    if (field_num == CAN_PAY_HK_HUM) {
        *tx_data = read_hum_raw_data();
    }

    else if (field_num == CAN_PAY_HK_PRES) {
        *tx_data = read_pres_raw_data();
    }

    else if (field_num == CAN_PAY_HK_AMB_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_GEN_THM);
    }

    else if (field_num == CAN_PAY_HK_6V_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BOOST6_TEMP);
    }

    else if (field_num == CAN_PAY_HK_10V_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BOOST10_TEMP);
    }

    else if (field_num == CAN_PAY_HK_MOT1_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_MOTOR_TEMP_1);
    }

    else if (field_num == CAN_PAY_HK_MOT2_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_MOTOR_TEMP_2);
    }

    // TODO - how are MF thermistors numbered?

    else if (field_num == CAN_PAY_HK_MF1_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF2_THM_6);
    }

    else if (field_num == CAN_PAY_HK_MF2_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF2_THM_5);
    }

    else if (field_num == CAN_PAY_HK_MF3_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF2_THM_4);
    }

    else if (field_num == CAN_PAY_HK_MF4_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF2_THM_3);
    }

    else if (field_num == CAN_PAY_HK_MF5_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF2_THM_2);
    }

    else if (field_num == CAN_PAY_HK_MF6_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF2_THM_1);
    }

    else if (field_num == CAN_PAY_HK_MF7_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF1_THM_6);
    }

    else if (field_num == CAN_PAY_HK_MF8_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF1_THM_5);
    }

    else if (field_num == CAN_PAY_HK_MF9_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF1_THM_4);
    }

    else if (field_num == CAN_PAY_HK_MF10_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF1_THM_3);
    }

    else if (field_num == CAN_PAY_HK_MF11_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF1_THM_2);
    }

    else if (field_num == CAN_PAY_HK_MF12_TEMP) {
        *tx_data = fetch_and_read_adc_channel(&adc2, ADC2_MF1_THM_1);
    }

    else if (field_num == CAN_PAY_HK_BAT_VOL) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BATT_VOLT_MON);
    }

    else if (field_num == CAN_PAY_HK_6V_VOL) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BOOST6_VOLT_MON);
    }

    else if (field_num == CAN_PAY_HK_6V_CUR) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BOOST6_CURR_MON);
    }

    else if (field_num == CAN_PAY_HK_10V_VOL) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BOOST10_VOLT_MON);
    }

    else if (field_num == CAN_PAY_HK_10V_CUR) {
        *tx_data = fetch_and_read_adc_channel(&adc1, ADC1_BOOST10_CURR_MON);
    }

    else if (field_num == CAN_PAY_HK_THERM_EN) {
        *tx_data = enables_to_uint(therm_enables, THERMISTOR_COUNT);
    }

    else if (field_num == CAN_PAY_HK_HEAT_EN) {
        *tx_data = enables_to_uint(heater_enables, HEATER_COUNT);
    }

    else if (field_num == CAN_PAY_HK_LSW_STAT) {
    }

    else if (field_num == CAN_PAY_HK_UPTIME) {
        *tx_data = uptime_s;
    }

    else if (field_num == CAN_PAY_HK_RESTART_COUNT) {
        *tx_data = restart_count;
    }

    else if (field_num == CAN_PAY_HK_RESTART_REASON) {
        *tx_data = restart_reason;
    }

    else {
        *tx_status = CAN_STATUS_INVALID_FIELD_NUM;
    }
}


void handle_opt(uint8_t field_num, uint8_t* tx_status) {
    // Check the field number is valid
    if (field_num >= CAN_PAY_OPT_TOT_FIELD_COUNT) {
        *tx_status = CAN_STATUS_INVALID_FIELD_NUM;
        return;
    }

    // TODO - refactor to be asynchronous
    // Get data from PAY-Optical over SPI
    // uint32_t tx_data = read_opt_spi(field_num);
}


void handle_ctrl(uint8_t field_num, uint32_t rx_data, uint8_t* tx_status,
        uint32_t* tx_data) {
    // TODO - fill in field implementations

    if (field_num == CAN_PAY_CTRL_PING) {
        // Don't do anything, just handle the field number so we send something back
    }

    else if (field_num == CAN_PAY_CTRL_ENABLE_6V) {
        enable_6V_boost();
    }

    else if (field_num == CAN_PAY_CTRL_DISABLE_6V) {
        disable_6V_boost();
    }

    else if (field_num == CAN_PAY_CTRL_ENABLE_10V) {
        enable_10V_boost();
    }

    else if (field_num == CAN_PAY_CTRL_DISABLE_10V) {
        disable_10V_boost();
    }

    else if (field_num == CAN_PAY_CTRL_GET_HEAT_SP) {
        *tx_data = heaters_setpoint_raw;
    }

    else if (field_num == CAN_PAY_CTRL_SET_HEAT_SP) {
        set_heaters_setpoint_raw((uint16_t) rx_data);
    }

    else if (field_num == CAN_PAY_CTRL_GET_THERM_READING) {
        if (rx_data < THERMISTOR_COUNT) {
            *tx_data = therm_readings_raw[rx_data];
        } else {
            *tx_status = CAN_STATUS_INVALID_DATA;
        }
    }

    else if (field_num == CAN_PAY_CTRL_GET_THERM_ERR_CODE) {
        if (rx_data < THERMISTOR_COUNT) {
            *tx_data = therm_err_codes[rx_data];
        } else {
            *tx_status = CAN_STATUS_INVALID_DATA;
        }
    }

    else if (field_num == CAN_PAY_CTRL_SET_THERM_ERR_CODE) {
        uint8_t therm_num = (rx_data >> 8) & 0xFF;  // byte 1
        uint8_t err_code = rx_data & 0xFF;          // byte 0

        if (therm_num < THERMISTOR_COUNT) {
            therm_err_codes[therm_num] = err_code;
        } else {
            *tx_status = CAN_STATUS_INVALID_DATA;
        }
    }

    else if (field_num == CAN_PAY_CTRL_MOTOR_DEP_ROUTINE) {
    }

    else if (field_num == CAN_PAY_CTRL_MOTOR_UP) {
        // forwards - up
        actuate_motors(40, 15, true);
    }

    else if (field_num == CAN_PAY_CTRL_MOTOR_DOWN) {
        // backwards - down
        actuate_motors(40, 15, false);
    }

    else if (field_num == CAN_PAY_CTRL_READ_EEPROM) {
        *tx_data = read_eeprom((uint16_t) rx_data);
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
        *tx_data = (uint32_t) (*pointer);
    }

    else if (field_num == CAN_PAY_CTRL_RESET_SSM) {
        reset_self_mcu(UPTIME_RESTART_REASON_RESET_CMD);
        // Note the program will stop here and restart
    }

    else if (field_num == CAN_PAY_CTRL_RESET_OPT) {
        rst_opt_spi();
    }

    else {
        *tx_status = CAN_STATUS_INVALID_FIELD_NUM;
    }
}

/*
If there is a TX message in the queue, sends it

When resume_mob(mob name) is called, it:
1) resumes the MOB
2) triggers an interrupt (callback function) to get the data to transmit
3) sends the data
4) pauses the mob
*/
void send_next_tx_msg(void) {
    if (queue_empty(&tx_msg_queue)) {
        return;
    }

    if (print_can_msgs) {
        uint8_t tx_msg[8] = { 0x00 };
        peek_queue(&tx_msg_queue, tx_msg);
        print("CAN TX: ");
        print_bytes(tx_msg, 8);
    }

    resume_mob(&cmd_tx_mob);
}
