#include <adc/adc.h>
#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/heaters.h"

// Set the temperature on both heaters to temp
void set_temp(double temp) {
    double resistance = therm_temp_to_res(temp);
    double voltage = therm_res_to_vol(resistance);
    print("Temperature = %f C, Resistance = %f kOhm, Voltage = %f V\n",
        temp, resistance, voltage);
    print("Setting temperature: %f C\n", temp);

    set_heaters_1_to_4_temp_setpoint(temp);
    set_heater_5_temp_setpoint(temp);
}

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

    // Cycle through a high and low set temperature 10 times
    // This is to check if the heater can turn on and off
    /*int count = 0;
    while (count < 10){
      // Heaters turn on
      print("Set temperature to 100\n");
      set_temp(100.0);
      _delay_ms(10000);

      // Heaters turn off
      print("Set temperature to 0\n");
      set_temp(0.0);
      _delay_ms(10000);

      count ++;
    }*/

    // CHANGE HERE FOR THERMISTORS CHANNEL
    uint8_t adc_channels[] = {0, 1, 5, 9};

    // automatically calculate the length of the array
    // uint8_t adc_channels_len = sizeof(adc_channels) / sizeof(adc_channels[0]);

    uint32_t seconds = 0;

    // CHANGE HERE FOR CHANNEL NUMBER
    // uint8_t thermister_channel_num = 4; // thermistor channels
    uint8_t thermister_channel_num = sizeof(adc_channels) / sizeof(adc_channels[0]);

    // Heaters should turn on once thermistors read something below this value
    set_temp(30.0); // Set temperature on the dac

    // Wait forever
    while (1) {
        // Display thermistor temperature reading to compare with the dac
        // set temperature - can measure voltage to see if the heaters turn on
        // once thermistor temperature reading exceeds the set dac temperature

        // Read all of the thermistors' voltage from adc
        fetch_all_adc_channels(&adc);
        // Find resistance for each channel
        print("\nThermistor Data\n");
        for (uint8_t i = 0; i < thermister_channel_num; i++) {
            uint8_t channel = adc_channels[i];
            print("Thermistor %d\n", channel+1);
            uint16_t raw_data = read_adc_channel(&adc, channel);
            double voltage = adc_raw_data_to_raw_vol(raw_data);
            double resistance = therm_vol_to_res(voltage);
            double thermis_temp = therm_res_to_temp(resistance);
            // Print temperature/resistance/voltage of thermistors
            print("Temperature: %f C, Resistance: %f kOhm, Voltage: %f V\n",
                thermis_temp, resistance, voltage);
        }

        _delay_ms(5000);
        seconds += 1;
        print("%lu seconds elapsed\n\n", seconds);
    }
}
