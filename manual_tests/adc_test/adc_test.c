#include <adc/adc.h>
#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/devices.h"


void read_voltage(char* name, uint8_t channel) {
    fetch_adc_channel(&adc, channel);
    uint16_t raw_data = read_adc_channel(&adc, channel);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    double voltage = adc_raw_data_to_eps_vol(raw_data);
    print("%s, %u, 0x%04x, %.6f V, %.6f V\n",
            name, channel, raw_data, raw_voltage, voltage);
}

void read_current(char* name, uint8_t channel) {
    fetch_adc_channel(&adc, channel);
    uint16_t raw_data = read_adc_channel(&adc, channel);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    double current = adc_raw_data_to_eps_cur(raw_data);
    print("%s, %u, 0x%04x, %.6f V, %.6f A\n",
            name, channel, raw_data, raw_voltage, current);
}

void read_therm(char* name, uint8_t channel) {
    fetch_adc_channel(&adc, channel);
    uint16_t raw_data = read_adc_channel(&adc, channel);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    double temp = adc_raw_data_to_therm_temp(raw_data);
    print("%s, %u, 0x%04x, %.6f V, %.6f C\n",
            name, channel, raw_data, raw_voltage, temp);
}


int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_adc(&adc);
    print("ADC Initialized\n");

    print("\nStarting test\n\n");

    while (1) {

        fetch_all_adc_channels(&adc);

        print("Name, Channel, Raw Data, Raw Voltage, Converted Data\n");

        //-----ADC1-----------------------------------------

        read_therm("ADC1 GEN THM",            ADC1_GEN_THM);

        read_voltage("ADC1 BOOST6 VOLT MON",  ADC1_BOOST6_VOLT_MON);
        read_therm("ADC1 MOTOR TEMP 1",       ADC1_MOTOR_TEMP_1);
        read_therm("ADC1 MOTOR TEMP 2",       ADC1_MOTOR_TEMP_2);
        read_therm("ADC1 BOOST6 TEMP",        ADC1_BOOST6_TEMP);
        read_voltage("ADC1 TBOOST10 TEMP",    ADC1_TBOOST10_TEMP);
        read_therm("ADC1 BOOST10 VOLT MON",   ADC1_BOOST10_VOLT_MON);
        read_current("ADC1 BOOST10 CURR MON", ADC1_BOOST10_CURR_MON);
        read_current("ADC1 BOOST6 CURR MON",  ADC1_BOOST6_CURR_MON);

        read_voltage("ADC1 BATT VOLT MON",    ADC1_BATT_VOLT_MON);
        
        //-----ADC2-----------------------------------------

        read_therm("ADC2 MF2 THM 6", ADC2_MF2_THM_6);
        read_therm("ADC2 MF2 THM 5", ADC2_MF2_THM_5);
        read_therm("ADC2 MF2 THM 4", ADC2_MF2_THM_4);
        read_therm("ADC2 MF2 THM 3", ADC2_MF2_THM_3);
        read_therm("ADC2 MF2 THM 2", ADC2_MF2_THM_2);
        read_therm("ADC2 MF2 THM 1", ADC2_MF2_THM_1);
        read_therm("ADC2 MF2 THM 6", ADC2_MF1_THM_6);
        read_therm("ADC2 MF2 THM 5", ADC2_MF1_THM_5);
        read_therm("ADC2 MF2 THM 4", ADC2_MF1_THM_4);
        read_therm("ADC2 MF2 THM 3", ADC2_MF1_THM_3);
        read_therm("ADC2 MF2 THM 2", ADC2_MF1_THM_2);
        read_therm("ADC2 MF2 THM 1", ADC2_MF1_THM_1);

        //--------------------------------------------------

        print("\n");
        _delay_ms(2000);
    }

    return 0;
}
