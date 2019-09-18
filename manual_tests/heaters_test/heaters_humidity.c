#include <adc/adc.h>
#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include "../../src/heaters.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_dac(&dac);
    print("DAC Initialized\n");

    init_adc(&adc);
    print("ADC Initialized\n");

    print("\nStarting test\n\n");

    // CHANGE HERE FOR THERMISTORS CHANNEL
    uint8_t adc_channels[] = {5, 6, 7, 8, 9};

    // automatically calculate the length of the array
    uint8_t thermister_channel_num = sizeof(adc_channels) / sizeof(adc_channels[0]);

    uint32_t seconds = 0;
    //not testing control loop, heaters always on
    double temp1_4 = 50.0;
    double temp5 = 50.0;

    // Wait forever
    print("\nThermistor Data\n");
    print("\nThermistor #, Temperature (C), Resistance (kohm), Voltage (V)\n");
    print("Setting temperature: %.3f C for heaters 1-4\n", temp1_4);
    set_heaters_1_to_4_temp_setpoint(temp1_4);
    print("Setting temperature: %.3f C for heaters 5\n", temp5);
    set_heater_5_temp_setpoint(temp5);
    while (1) {
        // Display thermistor temperature reading to compare with the dac
        // set temperature - can measure voltage to see if the heaters turn on
        // once thermistor temperature reading exceeds the set dac temperature

        // Read all of the thermistors voltage from adc
        fetch_all_adc_channels(&adc);
        // Find resistance for each channel
        for (uint8_t i = 0; i < thermister_channel_num; i++) {
            uint8_t channel = adc_channels[i];
            uint16_t raw_data = read_adc_channel(&adc, channel);
            double voltage = adc_raw_data_to_raw_vol(raw_data);
            double resistance = therm_vol_to_res(voltage);
            double thermis_temp = therm_res_to_temp(resistance);
            // Print temperature/resistance/voltage of thermistors
            print("%d, %.3f , %.3f, %.3f\n",
                channel+1, thermis_temp, resistance, voltage);
        }
        print("\n");
        _delay_ms(5000);
        seconds += 5;
    }
}
