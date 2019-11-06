#include <adc/adc.h>
#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/devices.h"


// Modify this array to contain the ADC channels you want to monitor
// (channels 10 and 11 are something else - motor positioning sensors)
// uint8_t adc_channels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
uint8_t adc_channels[] = {0, 1, 2};

// automatically calculate the length of the array
uint8_t adc_channels_len = sizeof(adc_channels) / sizeof(adc_channels[0]);

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_adc(&adc);
    print("ADC Initialized\n");

    print("adc_channels_len = %u\n", adc_channels_len);

    print("\nStarting test\n\n");

    //Find the temperature given the voltage output from the adc
    while (1) {
        //Read all of the thermistors' voltage from adc
        fetch_all_adc_channels(&adc);

        print("raw data, voltage (volts), resistance (kohms), temperature (C)\n");

        //Find resistance for each channel
        //only calculate it for the thermistors specified in adc_channels
        for (uint8_t i = 0; i < adc_channels_len; i++) {
            uint8_t channel = adc_channels[i];
            uint16_t raw_data = read_adc_channel(&adc, channel);
            double voltage = adc_raw_to_ch_vol(raw_data);
            //Convert adc voltage to resistance of thermistor
            double resistance = therm_vol_to_res(voltage);
            //Convert resistance to temperature of thermistor
            double temp = therm_res_to_temp(resistance);

            print("Channel %u: 0x%.3X, %f, %f, %f\n", channel, raw_data, voltage, resistance, temp);
        }

        print("\n");
        _delay_ms(2000);
    }

    return 0;
}
