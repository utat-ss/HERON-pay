#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include "../../src/heaters.h"

// Thermistors includes
#include <adc/adc.h>
#include <adc/pay.h>
#include <conversions/conversions.h>

// Thermistor adc
pin_info_t therm_cs = {
    .port = &ADC_CS_PORT_PAY,
    .ddr = &ADC_CS_DDR_PAY,
    .pin = ADC_CS_PIN_PAY
};

adc_t adc = {
    .channels = 0x0FFF,
    .cs = &therm_cs
};

extern dac_t dac;
extern pin_info_t dac_cs;
extern pin_info_t clr;

// Set the temperature on both heaters to temp
void set_temp(double temp) {
    double resistance = thermis_temp_to_resistance(temp);
    double voltage = thermis_resistance_to_voltage(resistance);
    print("Temperature = %f C, Resistance = %f kOhm, Voltage = %f V\n",
        temp, resistance, voltage);
    print("Setting temperature: %f C\n", temp);

    heaters_set_temp_a(temp);
    heaters_set_temp_b(temp);
}

// Get the resistance of the thermistor given the voltage
double thermis_voltage_to_resistance(double voltage) {
    return THERMIS_R_REF * (THERMIS_V_REF / voltage - 1);
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
    int count = 0;
    while (count < 10){
      // Heaters turn off
      print("Set temperature to 100\n");
      set_temp(100.0);
      _delay_ms(10000);

      // Heaters turn on
      print("Set temperature to 0\n");
      set_temp(0.0);
      _delay_ms(10000);

      count ++;
    }

    uint32_t seconds = 0;
    uint8_t thermister_channel_num = 3; // 3 thermistor channels
    set_temp(26.0); // Set temperature on the dac to 26C

    // Wait forever
    while (1) {
        // Display thermistor temperature reading to compare with the dac
        // set temperature - can measure voltage to see if the heaters turn on
        // once thermistor temperature reading exceeds the set dac temperature

        // Read all of the thermistors' voltage from adc
        fetch_all(&adc);
        // Find resistance for each channel
        print("\nThermistor Data\n");
        for (uint8_t i = 0; i < thermister_channel_num; i++) {
            print("Channel %d\n", i);
            uint16_t raw_data = read_channel(&adc, i);
            double voltage = adc_raw_data_to_raw_voltage(raw_data);
            double resistance = thermis_voltage_to_resistance(voltage);
            double thermis_temp = thermis_resistance_to_temp(resistance);
            // Print temperature/resistance/voltage of thermistors
            print("Temperature = %f C, Resistance = %f kOhm, Voltage = %f V\n",
                thermis_temp, resistance, voltage);
        }

        _delay_ms(1000);
        seconds += 1;
        print("%lu seconds elapsed\n\n", seconds);
    }
}
