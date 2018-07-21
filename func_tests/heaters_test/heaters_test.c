#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <util/delay.h>
#include "../../src/heaters.h"


void set_temp(double temp) {
    double resistance = thermis_temp_to_resistance(temp);
    double voltage = thermis_resistance_to_voltage(resistance);
    print("Temperature = %d C, Resistance = %d mOhm, Voltage = %d mV\n",
        (int16_t) temp, (int16_t) (resistance * 1000.0), (int16_t) (voltage * 1000.0));
    print("Setting temperature: %d C\n", (int16_t) temp);

    heaters_set_temp_a(temp);
    heaters_set_temp_b(temp);
}

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    dac_init(DAC_PAY);
    print("DAC Initialized\n");

    print("\nStarting test\n\n");

    // Set temperature to 30C
    set_temp(30.0);

    // Wait forever
    uint32_t minutes = 0;
    while (1) {
        _delay_ms(60000);
        minutes++;
        print("%lu minutes elapsed\n", minutes);
    }
}
