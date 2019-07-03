//################################################################################
//
// Pressure Vessel Test Code
// Written by: Dylan Vogel
//
// PAY-SSM "firmware" for running the pressure vessel test. Returns pressure,
// temperature and humidity readings from PAY-SSM. Intended to be used with 
// the corresponding Python file in:
// https://github.com/HeronMkII/test-software/tree/master/pressure-vessel-testing
//
// Please check that the code and board version match those below, or else I give
// no guarantees for code functionality.
//
//  PAY-SSM Version:    v3.0
//  Code Version:       v1.0
//
//################################################################################

#include <conversions/conversions.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include "../../src/env_sensors.h"

volatile uint8_t test_run = 0;

uint8_t rx_command(const uint8_t* buf, uint8_t len){
    if (buf[0] == 'A'){
        test_run = 1;
    } else if (buf[0] == 'F'){
        test_run = 0; 
    }
    return len;
}

int main(void) {
    init_uart();
    set_uart_rx_cb(rx_command);
    print("\n\n--UART initialized\n");

    init_spi();
    print("--SPI Initialized\n");

    init_temp();
    init_hum();
    init_pres();
    print("--Sensors Initialized\n");
    print("--Format: Pressure(kPA),Temperature(C),Humidity(%RH)\n");

    print("\n--Waiting for start command\n\n");

    while (1){
        if (test_run){
            uint16_t temp_raw = read_temp_raw_data();
            double temp = temp_raw_data_to_temperature(temp_raw);

            uint16_t hum_raw = read_hum_raw_data();
            double hum = hum_raw_data_to_humidity(hum_raw);

            uint32_t pres_raw = read_pres_raw_data();
            double pres = pres_raw_data_to_pressure(pres_raw);

            print("%.3f,%.3f,%.3f\n", pres, temp, hum);
        } else {
            print("%01X\n", 0xA);
        }
        _delay_ms(10000);
    }
}
