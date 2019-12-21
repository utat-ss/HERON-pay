#include <adc/adc.h>
#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/devices.h"


void read_voltage(char* name, uint8_t channel, double low_res, double high_res) {
    fetch_adc_channel(&adc1, channel);
    uint16_t raw_data = read_adc_channel(&adc1, channel);
    double raw_voltage = adc_raw_to_ch_vol(raw_data);
    double voltage = adc_raw_to_circ_vol(raw_data, low_res, high_res);
    print("%-25s, %u, 0x%03x, %.6f V, %.6f V\n",
            name, channel, raw_data, raw_voltage, voltage);
}

void read_current(char* name, uint8_t channel, double sense_res, double ref_vol) {
    fetch_adc_channel(&adc1, channel);
    uint16_t raw_data = read_adc_channel(&adc1, channel);
    double raw_voltage = adc_raw_to_ch_vol(raw_data);
    double current = adc_raw_to_circ_cur(raw_data, sense_res, ref_vol);
    print("%-25s, %u, 0x%03x, %.6f V, %.6f A\n",
            name, channel, raw_data, raw_voltage, current);
}

void read_therm(char* name, adc_t* adc, uint8_t channel) {
    fetch_adc_channel(adc, channel);
    uint16_t raw_data = read_adc_channel(adc, channel);
    double raw_voltage = adc_raw_to_ch_vol(raw_data);
    double temp = adc_raw_to_therm_temp(raw_data);
    print("%-25s, %u, 0x%03x, %.6f V, %.6f C\n",
            name, channel, raw_data, raw_voltage, temp);
}

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_adc(&adc1);
    init_adc(&adc2);
    print("ADC Initialized\n");

    print("\nStarting ADC Reading Test\n\n");

    while (1) {

        fetch_all_adc_channels(&adc1);
        fetch_all_adc_channels(&adc2);

        print("Name, Channel, Raw Data, Channel Voltage, Converted Data\n");

        //-----ADC1-----------------------------------------

        read_therm("ADC1 GEN THM",                 &adc1, ADC1_GEN_THM);

        read_therm("ADC1 MOTOR TEMP 1",            &adc1, ADC1_MOTOR_TEMP_1);
        read_therm("ADC1 MOTOR TEMP 2",            &adc1, ADC1_MOTOR_TEMP_2);

        read_voltage("ADC1 BOOST10 VOLT MON",      ADC1_BOOST10_VOLT_MON, ADC1_BOOST10_LOW_RES, ADC1_BOOST10_HIGH_RES);
        read_current("ADC1 BOOST10 CURR MON",      ADC1_BOOST10_CURR_MON, ADC1_BOOST10_SENSE_RES, ADC1_BOOST10_REF_VOL);
        read_therm("ADC1 BOOST10 TEMP",            &adc1, ADC1_BOOST10_TEMP);

        read_voltage("ADC1 BOOST6 VOLT MON",       ADC1_BOOST6_VOLT_MON, ADC1_BOOST6_LOW_RES, ADC1_BOOST6_HIGH_RES);
        read_current("ADC1 BOOST6 CURR MON",       ADC1_BOOST6_CURR_MON, ADC1_BOOST6_SENSE_RES, ADC1_BOOST6_REF_VOL);
        read_therm("ADC1 BOOST6 TEMP",             &adc1, ADC1_BOOST6_TEMP);

        read_voltage("ADC1 BATT VOLT MON",         ADC1_BATT_VOLT_MON, ADC1_BATT_LOW_RES, ADC1_BATT_HIGH_RES);
        
        //-----ADC2-----------------------------------------

        read_therm("ADC2 MF2 THM 6", &adc2, ADC2_MF2_THM_6);
        read_therm("ADC2 MF2 THM 5", &adc2, ADC2_MF2_THM_5);
        read_therm("ADC2 MF2 THM 4", &adc2, ADC2_MF2_THM_4);
        read_therm("ADC2 MF2 THM 3", &adc2, ADC2_MF2_THM_3);
        read_therm("ADC2 MF2 THM 2", &adc2, ADC2_MF2_THM_2);
        read_therm("ADC2 MF2 THM 1", &adc2, ADC2_MF2_THM_1);
        read_therm("ADC2 MF1 THM 6", &adc2, ADC2_MF1_THM_6);
        read_therm("ADC2 MF1 THM 5", &adc2, ADC2_MF1_THM_5);
        read_therm("ADC2 MF1 THM 4", &adc2, ADC2_MF1_THM_4);
        read_therm("ADC2 MF1 THM 3", &adc2, ADC2_MF1_THM_3);
        read_therm("ADC2 MF1 THM 2", &adc2, ADC2_MF1_THM_2);
        read_therm("ADC2 MF1 THM 1", &adc2, ADC2_MF1_THM_1);

        //--------------------------------------------------

        print("\n");
        _delay_ms(5000);
    }

    return 0;
}
