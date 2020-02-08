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


// 'sends' command (by putting it in the queue) to PAY
// PAY processes it and puts return data in CAN message in TX queue
// reads the TX queue and returns uint32_t data (from PAY)
uint32_t can_rx_tx(uint8_t op_code, uint8_t field_num, uint32_t rx_data){
    // initialize rx and tx messages
    uint8_t rx_msg[8] = {0x00};
    uint8_t tx_msg[8] = {0x00};

    // initialize queue size
    uint8_t rx_q_size = 0;
    uint8_t tx_q_size = 0;

    // load data
    rx_msg[0] = op_code;
    rx_msg[1] = field_num;
    rx_msg[2] = CAN_STATUS_OK;   // status byte
    rx_msg[3] = 0x00;   // unused
    tx_msg[4] = (rx_data >> 24) & 0xFF;
    tx_msg[5] = (rx_data >> 16) & 0xFF;
    tx_msg[6] = (rx_data >> 8) & 0xFF;
    tx_msg[7] = rx_data & 0xFF;

    // enqueue message
    enqueue(&rx_msg_queue, rx_msg);

    // check if message was enqueued
    rx_q_size = queue_size(&rx_msg_queue);
    tx_q_size = queue_size(&tx_msg_queue);
    ASSERT_EQ(rx_q_size, 1);
    ASSERT_EQ(tx_q_size, 0);

    // reads RX queue, performs requested operation
    // loads return data into TX queue
    process_next_rx_msg();

    // check data was successfully placed in TX queue
    rx_q_size = queue_size(&rx_msg_queue);
    tx_q_size = queue_size(&tx_msg_queue);
    ASSERT_EQ(rx_q_size, 0);
    ASSERT_EQ(tx_q_size, 1);

    // remove everything from queues
    dequeue(&tx_msg_queue, tx_msg);
    // print every CAN message
    // print("CAN TX: ");
    // print_bytes(tx_msg, 8);

    rx_q_size = queue_size(&rx_msg_queue);
    tx_q_size = queue_size(&tx_msg_queue);
    ASSERT_EQ(rx_q_size, 0);
    ASSERT_EQ(tx_q_size, 0);
    ASSERT_EQ(tx_msg[2], CAN_STATUS_OK);    // check status ok

    // return data
    uint32_t tx_data = ((uint32_t)tx_msg[4] << 24) | ((uint32_t)tx_msg[5] << 16) | ((uint32_t)tx_msg[6] << 8) | ((uint32_t)tx_msg[7]);
    return tx_data;
}

// 1
void hk_humidity_test(void) {
    uint32_t raw_humidity = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_HUM, 0x00);
    double humidity = hum_raw_data_to_humidity(raw_humidity);
    
    // reasonable room temperature humidity 5-20% (while sensor is under tape)
    ASSERT_BETWEEN(5, 20, humidity);
}

// 2
void hk_pressure_test(void) {
    uint32_t pressure_raw = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_PRES, 0x00);
    double pressure = pres_raw_data_to_pressure(pressure_raw);
    ASSERT_BETWEEN(98, 102, pressure);
}

// 3
void hk_amb_temp_test(void) {
    uint16_t temp_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_AMB_TEMP, 0x00);
    double temp = adc_raw_to_therm_temp(temp_raw);
    ASSERT_BETWEEN(15, 30, temp);
}

// 4
void hk_boost_6v_temp_test(void) {
    // don't need 32 bit precision for temperature
    // just use 16 bit, since the temp conversion function takes 16 bit
    uint16_t temp_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_TEMP, 0x00);
    double temp = adc_raw_to_therm_temp(temp_raw);
    ASSERT_BETWEEN(15, 30, temp);
}

// 5
void hk_boost_10v_temp_test(void) {
    uint16_t temp_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_10V_TEMP, 0x00);
    double temp = adc_raw_to_therm_temp(temp_raw);
    ASSERT_BETWEEN(15, 30, temp);
}

// 6
void hk_motor1_temp_test(void) {
    uint16_t temp_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_MOT1_TEMP, 0x00);
    double temp = adc_raw_to_therm_temp(temp_raw);
    ASSERT_BETWEEN(15, 30, temp);
}

// 7
void hk_motor2_temp_test(void) {
    uint16_t temp_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_MOT2_TEMP, 0x00);
    double temp = adc_raw_to_therm_temp(temp_raw);
    ASSERT_BETWEEN(-15, 30, temp);
}

// 8 
void hk_chip_temp_test(void) {
    for (uint8_t i=0; i<12; i++) {
        uint16_t temp_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, mf_temp_cmd[i], 0x00);
        double temp = adc_raw_to_therm_temp(temp_raw);

        // Lorna told me that Avinash said cells die at 40C
        ASSERT_BETWEEN(15, 40, temp);
    }
}

// 9
// fully charged battery voltage should be 4.0-4.2V
// but operating (nominal) voltage is 3.6V
void hk_battery_volt_test(void) {
    uint16_t volt_raw = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_BAT_VOL, 0x00);
    double volt = adc_raw_to_circ_vol(volt_raw, ADC1_BATT_LOW_RES, ADC1_BATT_HIGH_RES);
    ASSERT_BETWEEN(4.0, 4.2, volt);
}

// 10
void ctrl_6V_boost_voltage_test(void) {
    // enable boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_ENABLE_6V, 0x00);

    // delay 1 second for power to settle (for accurate reading)
    _delay_ms(1000);

    // get 6V boost converter voltage reading
    uint16_t raw_volt = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_6V_VOL, 0x00);
    double voltage = adc_raw_to_circ_vol (raw_volt, ADC1_BOOST6_LOW_RES, ADC1_BOOST6_HIGH_RES);

    ASSERT_BETWEEN(5.0, 7.0, voltage);

    // turn off boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_DISABLE_6V, 0x00);
    _delay_ms(1000);

    // check that voltage is between 2-4V to make sure that the boost converter is turned off
    raw_volt = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_6V_VOL, 0x00);
    voltage = adc_raw_to_circ_vol (raw_volt, ADC1_BOOST6_LOW_RES, ADC1_BOOST6_HIGH_RES);
    ASSERT_BETWEEN(2.0, 4.0, voltage);
}

// 11
void ctrl_6V_boost_current_test(void){
    // enable boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_ENABLE_6V, 0x00);

    // delay 1second for power to settle (for accurate reading)
    _delay_ms(1000);

    // get 6V boost converter current reading
    uint16_t raw_curr = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    double current = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    ASSERT_BETWEEN (0.0, 0.1, current);

    // turn off boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_DISABLE_6V, 0x00);
    _delay_ms(1000);

    // check that current is between 0-20mA to make sure that the boost converter is turned off
    // - reference, Lorna (06-Feb-2020)
    raw_curr = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    current = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    ASSERT_BETWEEN(0.0, 0.02, current);
}

// 12
void ctrl_10V_boost_voltage_test(void) {
    // enable boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_ENABLE_10V, 0x00);

    // delay 1 second for power to settle (for accurate reading)
    _delay_ms(1000);

    // get 10V boost converter voltage reading
    uint16_t raw_volt = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_10V_VOL, 0x00);
    double voltage = adc_raw_to_circ_vol (raw_volt, ADC1_BOOST10_LOW_RES, ADC1_BOOST10_HIGH_RES);

    ASSERT_BETWEEN(9.0, 12.0, voltage);

    // turn off boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_DISABLE_10V, 0x00);
    _delay_ms(1000);

    // check that voltage is between 2-4V to make sure that the boost converter is turned off
    raw_volt = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_10V_VOL, 0x00);
    voltage = adc_raw_to_circ_vol (raw_volt, ADC1_BOOST10_LOW_RES, ADC1_BOOST10_HIGH_RES);
    ASSERT_BETWEEN(2.0, 4.0, voltage);
}

// 13
void ctrl_10V_boost_current_test(void){
    // enable boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_ENABLE_10V, 0x00);

    // delay 3 seconds for power to settle (for accurate reading)
    _delay_ms(3000);

    // get 10V boost converter current reading
    uint16_t raw_curr = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_10V_CUR, 0x00);
    double current = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST10_SENSE_RES, ADC1_BOOST10_REF_VOL);
    ASSERT_BETWEEN (0.0, 0.05, current);

    // turn off boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_DISABLE_6V, 0x00);
    _delay_ms(3000);

    // check that current is between 0-20mA to make sure that the boost converter is turned off
    // - reference, Lorna (06-Feb-2020)
    raw_curr = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    current = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST10_SENSE_RES, ADC1_BOOST10_REF_VOL);
    ASSERT_BETWEEN(0.0, 0.02, current);
}

// 14
void hk_therm_status(void) {
    // run heater control so thermistor status return will be valid
    run_heater_ctrl();

    uint32_t therm_status = can_rx_tx(CAN_PAY_CTRL, CAN_PAY_HK_THERM_EN, 0x00);
    
    // should all be enabled
    ASSERT_EQ(therm_status, 0x0FFF);
}


// 15 (a)
// turn on heaters sequentially
// read the current (should see an increase) to make sure heaters are actually on
void hk_sequential_heater_test(void){
    double current_inc = 0;

    // enable 6V boost converter (powers heaters)
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_ENABLE_6V, 0x00);
    // delay 1 second for power to settle (for accurate reading)
    _delay_ms(1000);

    // read baseline current
    uint16_t raw_curr = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    double baseline_curr = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    ASSERT_BETWEEN(0.0, 0.02, baseline_curr);

    for (uint8_t i = 1; i<=5; i++){
        // turn on heater
        heater_on(i);

        // need delay here for current to start flowing?

        // read current with heaters on
        raw_curr = (uint16_t) can_rx_tx (CAN_PAY_CTRL, CAN_PAY_HK_6V_CUR, 0x00);
        current_inc = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL) - baseline_curr;
        
        // only 6V converters on draws 87mA
        if ((i==1) | (i==3)){
            // heater 1/3 + converter on draws 414mA
            ASSERT_BETWEEN(0.25, 0.35, current_inc);
        }
        else if((i==2) | (i==4)){
            // heater 2/4 + converter on draws 337mA
            ASSERT_BETWEEN(0.15, 0.25, current_inc);
        }
        else {  // heater 5
            // heater 5 + converter on draws 528mA
            ASSERT_BETWEEN(0.4, 0.6, current_inc);
        }

        heater_off(i);
    }

    // turn off boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_DISABLE_6V, 0x00);
    _delay_ms(1000);

    // check that current is between 0-20mA to make sure that the boost converter is turned off
    // - reference, Lorna (06-Feb-2020)
    raw_curr = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    double current = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    ASSERT_BETWEEN(0.0, 0.02, current);
}


// 15 (b)
// turn all heaters on
// check to see if current increased, to make sure heaters are on
void hk_all_heater_test(void){
    double current_inc = 0;

    // enable 6V boost converter (powers heaters)
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_ENABLE_6V, 0x00);
    // delay 1 second for power to settle (for accurate reading)
    _delay_ms(1000);

    // read baseline current
    uint16_t raw_curr = (uint16_t) can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    double baseline_curr = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    ASSERT_BETWEEN(0.0, 0.04, baseline_curr);

    // turn all heaters on, wait 1 second, then read current
    heater_all_on();
    _delay_ms(5000);
    raw_curr = (uint16_t) can_rx_tx(CAN_PAY_CTRL, CAN_PAY_HK_6V_CUR, 0x00);
    current_inc = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL) - baseline_curr;
    
    // expected increase ~1.0 - 1.5A
    ASSERT_BETWEEN(1.0, 1.5, current_inc);

    heater_all_off();

    // turn off boost converter
    can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_DISABLE_6V, 0x00);
    _delay_ms(1000);

    // check that current is between 0-20mA to make sure that the boost converter is turned off
    // - reference, Lorna (06-Feb-2020)
    raw_curr = (uint16_t) can_rx_tx (CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    double current = adc_raw_to_circ_cur(raw_curr, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
    ASSERT_BETWEEN(0.0, 0.02, current);
}


// 16
void ctrl_limit_switch_status(void){
    uint8_t lsw_status = (uint8_t) can_rx_tx(CAN_PAY_CTRL, CAN_PAY_CTRL_GET_LSW_STATUS, 0x00);

    // returns 2 bits, bit[1] = bottom 2, bit[0] = bottom 1
    // 0 = not pressed, 1 = pressed
    ASSERT_EQ(lsw_status, 0x0);
}

/*
missing tests from BUS architecture doc
- 17 = uptime
- 18 = restart count
- 19 = restart reason
*/

test_t t1 = { .name = "1. humidity sensor test", .fn = hk_humidity_test };
test_t t2 = { .name = "2. pressure sensor test", .fn = hk_pressure_test };
test_t t3 = { .name = "3. ambient temp test", .fn = hk_amb_temp_test };
test_t t4 = { .name = "4. 6V boost temp test", .fn = hk_boost_6v_temp_test };
test_t t5 = { .name = "5. 10V boost temp test", .fn = hk_boost_10v_temp_test };
test_t t6 = { .name = "6. motor 1 temp test", .fn = hk_motor1_temp_test };
test_t t7 = { .name = "7. motor 2 temp test", .fn = hk_motor2_temp_test };
test_t t8 = { .name = "8. mf chip temp test", .fn = hk_chip_temp_test };
test_t t9 = { .name = "9. battery volt test", .fn = hk_battery_volt_test };
test_t t10 = { .name = "10. 6V boost voltage test", .fn = ctrl_6V_boost_voltage_test };
test_t t11 = { .name = "11. 6V boost current test", .fn = ctrl_6V_boost_current_test };
test_t t12 = { .name = "12. 10V boost voltage test", .fn = ctrl_10V_boost_voltage_test };
test_t t13 = { .name = "13. 10V boost current test", .fn = ctrl_10V_boost_current_test };
test_t t14 = { .name = "14. thermistor status test", .fn = hk_therm_status };
test_t t15a = { .name = "15a. sequential heater test", .fn = hk_sequential_heater_test };
test_t t15b = { .name = "15b. all heater test", .fn = hk_all_heater_test };
test_t t16 = { .name = "16. limit switch test", .fn = ctrl_limit_switch_status };


test_t* suite[] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10, &t11, &t12, &t13, &t14, &t15a, &t15b, &t16 };
test_t* temp[] = { &t13, &t14, &t15a, &t15b, &t16 };

int main(void) {
    init_pay();

    // run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    run_tests(temp, sizeof(temp) / sizeof(temp[0]));
    return 0;
}
