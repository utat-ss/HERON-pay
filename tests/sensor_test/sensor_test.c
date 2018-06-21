#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/uart.h>
#include <spi/spi.h>
#include <conversions/conversions.h>
#include <util/delay.h>
#include "../../src/sensors.h"

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    temp_init();
    hum_init();
    pres_init();
    print("Sensors Initialized\n");

    print("\nStarting test\n\n");

    while (1) {
        uint16_t temp_raw = temp_read_raw_data();
        float temp = temp_convert_raw_data_to_temperature(temp_raw);
        print("Temperature: %4x = %d C\n", temp_raw, (int8_t) temp);

        uint16_t hum_raw = hum_read_raw_humidity();
        double hum = hum_convert_raw_humidity_to_humidity(hum_raw);
        print("Humidity: %4x = %d %%RH\n", hum_raw, (int8_t) hum);

        uint32_t pres_raw = pres_read_raw_pressure();
        float pres = pres_convert_raw_pressure_to_pressure(pres_raw);
        print("Pressure: %8x = %d kPa\n", pres_raw, (int8_t) pres);

        print("\n");
        _delay_ms(10000);
    }
}
