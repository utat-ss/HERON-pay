#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/boost.h"
#include "../../src/devices.h"

void read_voltage(char* name, uint8_t channel) {
    fetch_adc_channel(&adc1, channel);
    uint16_t raw_data = read_adc_channel(&adc1, channel);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    double voltage = adc_raw_data_to_eps_vol(raw_data);
    print("%s, %u, 0x%04x, %.6f V, %.6f V\n",
            name, channel, raw_data, raw_voltage, voltage);
}

void read_current(char* name, uint8_t channel) {
    fetch_adc_channel(&adc1, channel);
    uint16_t raw_data = read_adc_channel(&adc1, channel);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    double current = adc_raw_data_to_eps_cur(raw_data);
    print("%s, %u, 0x%04x, %.6f V, %.6f A\n",
            name, channel, raw_data, raw_voltage, current);
}

void read_therm_adc1(char* name, uint8_t channel) {
    fetch_adc_channel(&adc1, channel);
    uint16_t raw_data = read_adc_channel(&adc1, channel);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    double temp = adc_raw_data_to_therm_temp(raw_data);
    print("%s, %u, 0x%04x, %.6f V, %.6f C\n",
            name, channel, raw_data, raw_voltage, temp);
}

uint8_t count = 5; //take 5 readings

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 't':
            //"t"en
            print("Enabled 10V boost\n");
            enable_10V_boost();

            _delay_ms(2000);
            while(count > 0){
              fetch_all_adc_channels(&adc1);
              print("Name, Channel, Raw Data, Raw Voltage, Converted Data\n");
              read_voltage("ADC1 BOOST10 VOLT MON",      ADC1_BOOST10_VOLT_MON);
              read_current("ADC1 BOOST10 CURR MON",      ADC1_BOOST10_CURR_MON);
              read_therm_adc1("ADC1 BOOST10 TEMP",       ADC1_BOOST10_TEMP);
              count--;
              _delay_ms(1000);
            }
            count = 5;
            break;
        case 's':
            //"s"ix
            print("Enabled 6V boost\n");
            enable_6V_boost();

            _delay_ms(2000);
            while(count > 0){
              fetch_all_adc_channels(&adc1);
              print("Name, Channel, Raw Data, Raw Voltage, Converted Data\n");
              read_voltage("ADC1 BOOST6 VOLT MON",       ADC1_BOOST6_VOLT_MON);
              read_current("ADC1 BOOST6 CURR MON",       ADC1_BOOST6_CURR_MON);
              read_therm_adc1("ADC1 BOOST6 TEMP",        ADC1_BOOST6_TEMP);
              count--;
              _delay_ms(1000);
            }
            count = 5;
            break;
        case 'q':
            //arbitrary
            print("Disabling 10V boost\n");
            disable_10V_boost();
            break;
        case 'w':
            //arbitrary
            print("Disabling 6V boost\n");
            disable_6V_boost();
            break;
        case 'e':
            //"e"mergency stop if anything goes wrong
            print("Disabling both boost\n");
            disable_10V_boost();
            disable_6V_boost();
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("\n\nSPI initialized\n");

    init_pex(&pex2);
    print("\n\nPEX2 initialized\n");

    init_boosts();
    print("Boost PEX initialized\n");

    init_adc(&adc1);
    init_adc(&adc2);
    print("ADC Initialized\n");

    print("\nStarting Boost test\n\n");

    set_uart_rx_cb(key_pressed);
    while (1) {}
}
