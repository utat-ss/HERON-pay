#include <adc/adc.h>
#include <conversions/conversions.h>
#include <can/data_protocol.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/general.h"


uint32_t last_col_uptime_s = 0;
const uint32_t col_period_s = 30;

// 'sends' command (by putting it in the queue) to PAY
// PAY processes it and puts return data in CAN message in TX queue
// reads the TX queue and returns uint32_t data (from PAY)
uint32_t can_rx_tx(uint8_t op_code, uint8_t field_num, uint32_t rx_data){
    // initialize rx message
    uint8_t rx_msg[8] = {0x00};

    // load data
    rx_msg[0] = op_code;
    rx_msg[1] = field_num;
    rx_msg[2] = CAN_STATUS_OK;   // status byte
    rx_msg[3] = 0x00;   // unused
    rx_msg[4] = (rx_data >> 24) & 0xFF;
    rx_msg[5] = (rx_data >> 16) & 0xFF;
    rx_msg[6] = (rx_data >> 8) & 0xFF;
    rx_msg[7] = rx_data & 0xFF;

    // enqueue message
    enqueue(&rx_msg_queue, rx_msg);

    // reads RX queue, performs requested operation
    // loads return data into TX queue
    process_next_rx_msg();

    // remove everything from queues
    uint8_t tx_msg[8] = {0x00};
    dequeue(&tx_msg_queue, tx_msg);

    // print every CAN message
    // print("CAN TX: ");
    // print_bytes(tx_msg, 8);

    // return data
    uint32_t tx_data =
        ((uint32_t)tx_msg[4] << 24) |
        ((uint32_t)tx_msg[5] << 16) |
        ((uint32_t)tx_msg[6] << 8) |
        ((uint32_t)tx_msg[7]);
    return tx_data;
}

void print_str(char* value) {
    // right justify
    print(", %15s", value);
}

void print_double(double value) {
    print(", %15.6f", value);
}

void print_int(uint32_t value) {
    print(", %15lu", value);
}


void print_header(void) {
    print_str("Uptime (s)");
    print_str("Restart count");
    print_str("Restart reason");
    print_str("Humidity (%RH)");
    print_str("Pressure (kPa)");
    print_str("Amb Temp (C)");
    print_str("6V Temp (C)");
    print_str("10V Temp (C)");
    print_str("Mot 1 Temp (C)");
    print_str("Mot 2 Temp (C)");
    print_str("MF Temp 1 (C)");
    print_str("MF Temp 2 (C)");
    print_str("MF Temp 3 (C)");
    print_str("MF Temp 4 (C)");
    print_str("MF Temp 5 (C)");
    print_str("MF Temp 6 (C)");
    print_str("MF Temp 7 (C)");
    print_str("MF Temp 8 (C)");
    print_str("MF Temp 9 (C)");
    print_str("MF Temp 10 (C)");
    print_str("MF Temp 11 (C)");
    print_str("MF Temp 12 (C)");
    print_str("Bat Vol (V)");
    print_str("6V Vol (V)");
    print_str("6V Cur (A)");
    print_str("10V Vol (V)");
    print_str("10V Cur (A)");
    print("\n");
}

void collect_data(void) {
    uint32_t uptime = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_UPTIME, 0x00);
    print_int(uptime);

    uint32_t count = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_RESTART_COUNT, 0x00);
    print_int(count);

    uint32_t reason = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_RESTART_REASON, 0x00);
    print_int(reason);

    uint32_t raw_humidity = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_HUM, 0x00);
    print_double(hum_raw_data_to_humidity(raw_humidity));

    uint32_t pressure_raw = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_PRES, 0x00);
    print_double(pres_raw_data_to_pressure(pressure_raw));

    uint32_t temp_raw_amb = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_AMB_TEMP, 0x00);
    print_double(adc_raw_to_therm_temp(temp_raw_amb));

    uint32_t temp_raw_6v = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_TEMP, 0x00);
    print_double(adc_raw_to_therm_temp(temp_raw_6v));

    uint32_t temp_raw_10v = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_10V_TEMP, 0x00);
    print_double(adc_raw_to_therm_temp(temp_raw_10v));

    uint32_t temp_raw_mot1 = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_MOT1_TEMP, 0x00);
    print_double(adc_raw_to_therm_temp(temp_raw_mot1));

    uint32_t temp_raw_mot2 = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_MOT2_TEMP, 0x00);
    print_double(adc_raw_to_therm_temp(temp_raw_mot2));

    for (uint8_t i=0; i<THERMISTOR_COUNT; i++) {
        uint32_t temp_raw_mf = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_MF1_TEMP + i, 0x00);
        print_double(adc_raw_to_therm_temp(temp_raw_mf));
    }

    uint32_t volt_raw_bat = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_BAT_VOL, 0x00);
    print_double(adc_raw_to_circ_vol(volt_raw_bat, ADC1_BATT_LOW_RES, ADC1_BATT_HIGH_RES));

    uint32_t raw_volt_6v = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_VOL, 0x00);
    print_double(adc_raw_to_circ_vol(raw_volt_6v, ADC1_BOOST6_LOW_RES, ADC1_BOOST6_HIGH_RES));

    uint32_t raw_curr_6v = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_6V_CUR, 0x00);
    print_double(adc_raw_to_circ_cur(raw_curr_6v, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL));

    uint32_t raw_volt_10v = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_10V_VOL, 0x00);
    print_double(adc_raw_to_circ_vol(raw_volt_10v, ADC1_BOOST10_LOW_RES, ADC1_BOOST10_HIGH_RES));

    uint32_t raw_curr_10v = can_rx_tx(CAN_PAY_HK, CAN_PAY_HK_10V_CUR, 0x00);
    print_double(adc_raw_to_circ_cur(raw_curr_10v, ADC1_BOOST10_SENSE_RES, ADC1_BOOST10_REF_VOL));

    print("\n");
}

void print_time(void) {
    print("Current time: %lu s\n", uptime_s);
    print("Last collection time: %lu s\n", last_col_uptime_s);
    print("Collection period: %lu s\n", col_period_s);
}

void heaters_off(void) {
    heater_all_off();
    print("HEATERS OFF\n");
}

void heaters_on(void) {
    heater_all_on();
    print("HEATERS ON\n");
}


void print_cmds(void) {
    print("1. Collect data now\n");
    print("2. Print data header\n");
    print("3. Print time\n");
    print("4. Resync data collection timer to start now\n");
    print("5. Turn heaters off\n");
    print("6. Turn heaters on\n");
    print("h. Help\n");
    print("\n");
}


int main(void){
    init_pay();
    print("\n\n");
    print("Initialized PAY\n");

    print_can_msgs = false;

    print("\nStarting test\n\n");

    heaters_off();
    print_cmds();
    print_header();

    while (1) {
        if (get_uart_rx_count() > 0) {
            uint8_t cmd = get_uart_rx_buf()[0];
            clear_uart_rx_buf();
            print("Got command: %c\n", cmd);

            if        (cmd == '1') {
                collect_data();
            } else if (cmd == '2') {
                print_header();
            } else if (cmd == '3') {
                print_time();
            } else if (cmd == '4') {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    last_col_uptime_s = uptime_s;
                }
                print_time();
            } else if (cmd == '5') {
                heaters_off();
            } else if (cmd == '6') {
                heaters_on();
            } else if (cmd == 'h') {
                print_cmds();
            } else {
                print("INVALID COMMAND\n");
            }
        }

        bool col_now = false;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            if (uptime_s >= last_col_uptime_s + col_period_s) {
                last_col_uptime_s = uptime_s;
                col_now = true;
            }
        }
        if (col_now) {
            collect_data();
        }
    }

    return 0;
}
