// Standard libraries
#include <stdlib.h>

// lib-common libraries
#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <adc/adc.h>
#include <test/test.h>
#include <can/data_protocol.h>
#include <conversions/conversions.h>

// PAY libraries
#include "../../src/general.h"

// Field numbers for pay hk
uint8_t mf_temp_cmd[12] = {CAN_PAY_HK_MF1_TEMP, CAN_PAY_HK_MF2_TEMP, \
    CAN_PAY_HK_MF3_TEMP, CAN_PAY_HK_MF4_TEMP, CAN_PAY_HK_MF4_TEMP, \
    CAN_PAY_HK_MF6_TEMP, CAN_PAY_HK_MF7_TEMP, CAN_PAY_HK_MF8_TEMP, \
    CAN_PAY_HK_MF9_TEMP, CAN_PAY_HK_MF10_TEMP, CAN_PAY_HK_MF11_TEMP, \
    CAN_PAY_HK_MF12_TEMP};

#define ASSERT_BETWEEN(least, greatest, value) \
    ASSERT_FP_GREATER(value, least - 0.001); \
    ASSERT_FP_LESS(value, greatest + 0.001);

float get_pay_hk(uint8_t field_num) {
    uint8_t rx_msg[8] = { 0x00 };
    rx_msg[2] = CAN_PAY_HK;
    rx_msg[3] = field_num;
    enqueue(&rx_msg_queue, rx_msg);
    process_next_rx_msg();
    uint8_t tx_msg[8] = {0x00};
    dequeue(&tx_msg_queue, tx_msg);
    uint32_t tx_data =
        ((uint32_t) tx_msg[4] << 24) |
        ((uint32_t) tx_msg[5] << 16) |
        ((uint32_t) tx_msg[6] << 8) |
        ((uint32_t) tx_msg[7]);
    return tx_data;
}

void send_pay_ctrl(uint8_t field_num) {
    uint8_t rx_msg[8] = { 0x00 };
    rx_msg[2] = CAN_PAY_CTRL;
    rx_msg[3] = field_num;
    enqueue(&rx_msg_queue, rx_msg);
    process_next_rx_msg();
    uint8_t tx_msg[8] = {0x00};
    dequeue(&tx_msg_queue, tx_msg);
    //TODO - assert for valid message?
}

// For boost only
float process_pay_hk(uint8_t field_num, uint32_t tx_data) {
    if (field_num == CAN_PAY_HK_HUM){
        return hum_raw_data_to_humidity(tx_data);
    }
    else if (field_num == CAN_PAY_HK_PRES) {
        return pres_raw_data_to_pressure(tx_data);
    }
    else if (field_num == CAN_PAY_HK_AMB_TEMP) {
        return temp_raw_data_to_temperature(tx_data);
    }
    else if (field_num == CAN_PAY_HK_6V_TEMP) {
        return -1; //Not implemented yet
    }
    else if (field_num == CAN_PAY_HK_10V_TEMP) {
        return -1; //Not implemented yet
    }
    else if (field_num == CAN_PAY_HK_MOT1_TEMP) {
        return -1; //Not implemented yet
    }
    else if (field_num == CAN_PAY_HK_MOT2_TEMP) {
        return -1; //Not implemented yet
    }
    else if (field_num == CAN_PAY_HK_BAT_VOL) {
        return -1; //Not implemented yet
    }
    else if (field_num == CAN_PAY_HK_10V_VOL) {
        return adc_raw_to_circ_vol(tx_data, ADC1_BOOST10_LOW_RES, ADC1_BOOST10_HIGH_RES);
    }
    else if (field_num == CAN_PAY_HK_6V_CUR) {
        return adc_raw_to_circ_cur(tx_data, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    }
    else if (field_num == CAN_PAY_HK_10V_CUR) {
        return adc_raw_to_circ_cur(tx_data, ADC1_BOOST10_SENSE_RES, ADC1_BOOST10_REF_VOL);
    }
    return 0x00;
}

void hk_humidity_test(void) {
    uint32_t raw_humidity = get_pay_hk(CAN_PAY_HK_HUM);
    double humidity = process_pay_hk(CAN_PAY_HK_HUM, raw_humidity);
    ASSERT_BETWEEN(98, 100, humidity);
}

void hk_pressure_test(void) {
    uint32_t pressure_raw = get_pay_hk(CAN_PAY_HK_PRES);
    double pressure = process_pay_hk(CAN_PAY_HK_PRES, pressure_raw);
    ASSERT_BETWEEN(98, 102, pressure);
}

void hk_amb_temp_test(void) {
    uint16_t temp_raw = get_pay_hk(CAN_PAY_HK_AMB_TEMP);
    double temp = process_pay_hk(CAN_PAY_HK_AMB_TEMP, temp_raw);
    ASSERT_BETWEEN(-39, 80, temp);
}

void hk_boost_6v_temp_test(void) {
    uint32_t temp_raw = get_pay_hk(CAN_PAY_HK_6V_TEMP);
    double temp = process_pay_hk(CAN_PAY_HK_6V_TEMP, temp_raw);
    ASSERT_BETWEEN(0, 90, temp);
}

void hk_boost_10v_temp_test(void) {
    uint32_t temp_raw = get_pay_hk(CAN_PAY_HK_10V_TEMP);
    double temp = process_pay_hk(CAN_PAY_HK_10V_TEMP, temp_raw);
    ASSERT_BETWEEN(0, 90, temp);
}

void hk_motor1_temp_test(void) {
    uint32_t temp_raw = get_pay_hk(CAN_PAY_HK_MOT1_TEMP);
    double temp = process_pay_hk(CAN_PAY_HK_MOT1_TEMP, temp_raw);
    ASSERT_BETWEEN(-39, 80, temp);
}

void hk_motor2_temp_test(void) {
    uint32_t temp_raw = get_pay_hk(CAN_PAY_HK_MOT2_TEMP);
    double temp = process_pay_hk(CAN_PAY_HK_MOT2_TEMP, temp_raw);
    ASSERT_BETWEEN(-39, 80, temp);
}

void hk_chip_temp_test(void) {
    for (uint8_t i=0; i<12; i++) {
        uint32_t temp_raw = get_pay_hk(mf_temp_cmd[i]);
        // TODO - this conversion func
        double temp = -1;
        ASSERT_BETWEEN(35, 39, temp);
    }
}

void hk_battery_volt_test(void) {
    uint32_t volt_raw = get_pay_hk(CAN_PAY_HK_BAT_VOL);
    double volt = process_pay_hk(CAN_PAY_HK_BAT_VOL, volt_raw);
    ASSERT_BETWEEN(3, 4.2, volt);
}

void ctrl_boost_10v_volt_test(void) {
    // TODO - I made this up
    // Enable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_10V);
    uint32_t raw_volt = get_pay_hk(CAN_PAY_HK_10V_VOL);
    double volt = process_pay_hk(CAN_PAY_HK_10V_VOL, raw_volt);
    ASSERT_BETWEEN(9, 12, volt);
    // Disable Boost Converter
    send_pay_ctrl(CAN_PAY_CTRL_DISABLE_10V);
}

void ctrl_boost_10v_motor_curr_test(void) {
    // TODO - I made this up
    // Enable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_10V);
    uint32_t raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
    double curr = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr);
    ASSERT_BETWEEN(0, 3.6, curr);
    // Move a motor
    send_pay_ctrl(CAN_PAY_CTRL_MOTOR_UP);
    raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
    curr = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr);
    ASSERT_BETWEEN(0, 3.6, curr);
    // TODO - How do you stop the motor
}

// TODO - Some tests about status
void hk_therm_status(void) {
    uint32_t therm_status = get_pay_hk(CAN_PAY_HK_THERM_STAT);
    ASSERT_EQ(therm_status, 0x0FFF);
}
// TODO - Limit switch stuff

void ctrl_10V_boost_test(void) {
    float current_inc;
    // Enable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_10V);
    // Get 10V boost current measurement
    uint32_t raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
    float baseline_current = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr);

    for (uint8_t i = 1; i <= 5; i++) {
        // Turn on heater
        heater_on(i);
        // Check current increased between 0.15A and 0.2A
        raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
        current_inc = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr) - baseline_current;
        ASSERT_BETWEEN(0.15, 0.2, current_inc);
        // Turn off heater
        heater_off(i);
    }

    // Turn on all heaters
    heater_all_on();
    raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
    current_inc = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr) - baseline_current;
    ASSERT_BETWEEN(0.75, 1, current_inc);

    heater_all_off();
    // Disable Boost Converter
    send_pay_ctrl(CAN_PAY_CTRL_DISABLE_10V);
}

void ctrl_6V_boost_test(void) {
    // Enable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_6V);
    // Turn on all heaters
    heater_all_on();
    uint32_t raw_curr = get_pay_hk(CAN_PAY_HK_6V_CUR);
    float baseline_current = process_pay_hk(CAN_PAY_HK_6V_CUR, raw_curr);
    // Disable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_DISABLE_6V);
    raw_curr = get_pay_hk(CAN_PAY_HK_6V_CUR);
    float current_dec = baseline_current - process_pay_hk(CAN_PAY_HK_6V_CUR, raw_curr);
    ASSERT_BETWEEN(-0.05, 0.05, current_dec);
    heater_all_off();
}

void ctrl_temp_lowpower_test(void) {
    // Enable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_10V);
    // Start temp low-power mode
    // send_pay_ctrl(CAN_PAY_CTRL_START_TEMP_LPM); //TODO - Does this not exist anymore?
    uint32_t raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
    float curr = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr);
    ASSERT_BETWEEN(0.1, 0.2, curr);
    // Disable Boost Converter
    send_pay_ctrl(CAN_PAY_CTRL_DISABLE_10V);
}

void ctrl_indef_lowpower_test(void) {
    // Enable Boost converter
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_10V);
    // Start temp low-power mode
    send_pay_ctrl(CAN_PAY_CTRL_ENABLE_INDEF_LPM);
    uint32_t raw_curr = get_pay_hk(CAN_PAY_HK_10V_CUR);
    float curr = process_pay_hk(CAN_PAY_HK_10V_CUR, raw_curr);
    ASSERT_BETWEEN(0.1, 0.15, curr);
    // Stop indef low power mode
    send_pay_ctrl(CAN_PAY_CTRL_DISABLE_INDEF_LPM);
    // Disable Boost Converter
    send_pay_ctrl(CAN_PAY_CTRL_DISABLE_10V);
}

test_t t1 = { .name = "humidity sensor test", .fn = hk_humidity_test };
test_t t2 = { .name = "pressure sensor test", .fn = hk_pressure_test };

// Stuff that potentially doesn't work
test_t t3 = { .name = "ambient temp test", .fn = hk_amb_temp_test };
test_t t4 = { .name = "6V boost temp test", .fn = hk_boost_6v_temp_test };
test_t t5 = { .name = "10V boost temp test", .fn = hk_boost_10v_temp_test };
test_t t6 = { .name = "motor 1 temp test", .fn = hk_motor1_temp_test };
test_t t7 = { .name = "motor 2 temp test", .fn = hk_motor2_temp_test };
test_t t8 = { .name = "mf chip temp test", .fn = hk_chip_temp_test };
test_t t9 = { .name = "battery volt test", .fn = hk_battery_volt_test };
test_t t10 = { .name = "10V boost volt test", .fn = ctrl_boost_10v_volt_test };
test_t t11 = { .name = "10V motor current test", .fn = ctrl_boost_10v_motor_curr_test };
test_t t12 = { .name = "thermistor status test", .fn = hk_therm_status };
test_t t13 = { .name = "10V boost ctrl test", .fn = ctrl_10V_boost_test };
test_t t14 = { .name = "6V boost ctrl test", .fn = ctrl_6V_boost_test };
test_t t15 = { .name = "temp low power ctrl test", .fn = ctrl_temp_lowpower_test };
test_t t16 = { .name = "indef low power ctrl test", .fn = ctrl_indef_lowpower_test };

test_t* suite[] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10, &t11, &t12, \
    &t13, &t14, &t15, &t16 };

int main(void) {
    init_pay();

    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    return 0;
}
