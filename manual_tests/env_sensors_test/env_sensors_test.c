#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>

#include "../../src/env_sensors.h"

int main(void) {
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
        double temp = temp_raw_data_to_temperature(temp_raw);
        print("Temperature: 0x%04X = %d C\n", temp_raw, (int8_t) temp);

        uint16_t hum_raw = hum_read_raw_data();
        double hum = hum_raw_data_to_humidity(hum_raw);
        print("Humidity: 0x%04X = %d %%RH\n", hum_raw, (int8_t) hum);

        uint32_t pres_raw = pres_read_raw_data();
        double pres = pres_raw_data_to_pressure(pres_raw);
        print("Pressure: 0x%08lX (%lu) = %d kPa\n", pres_raw, pres_raw, (int16_t) pres);

        print("\n");
        _delay_ms(5000);
    }
}
