#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/log.h>
#include <util/delay.h>
#include <stdint.h>
#include "../../src/analog_temp.h"

int main(void){
    uint16_t data;                 // stores the raw ADC data
    double resistance, temp;       // stores calculated values

    init_uart();
    print("UART initialized\n");

    print("Running thermistor testing code\n");

    init_thermistor();

    while(1){
        int i = 0;
        data = read_thermistor_adc(i);        // FIXME: change to desired channel
        print("Raw ADC value: %lX\n", (uint32_t)(data));

        resistance = convert_thermistor_reading(data, i);
        print("Resistance (Ohm): %u\n", (int)(resistance * 1000));
        print("Reference Channel: %u\n", (int)(R_REF[i]));

        temp = resistance_to_temp(resistance);
        print("Temperature (mC): %u\n", (int)(temp * 1000));

        print("\n\n");
        _delay_ms(1000);
    }
}
