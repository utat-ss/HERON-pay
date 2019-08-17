// Standard libraries
#include <stdlib.h>

// lib-common libraries
#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <adc/adc.h>
#include <test/test.h>
#include <can/data_protocol.h>

// PAY libraries
#include "../../src/devices.h"
#include "../../src/env_sensors.h"
#include "../../src/optical_spi.h"

#define ASSERT_BETWEEN(least, greatest, value) \
    ASSERT_FP_GREATER(value, least); \
    ASSERT_FP_LESS(value, greatest);

void temperature_test(void) {
    uint16_t temp_raw = read_temp_raw_data();
    double temp = temp_raw_data_to_temperature(temp_raw);
    ASSERT_BETWEEN(25, 37, (uint8_t) temp);
}

void humidity_test(void) {
    uint16_t hum_raw = read_hum_raw_data();
    double hum = hum_raw_data_to_humidity(hum_raw);
    ASSERT_BETWEEN(95, 101, (uint8_t) hum);
}

void pressure_test(void) {
    uint32_t pres_raw = read_pres_raw_data();
    double pres = pres_raw_data_to_pressure(pres_raw);
    ASSERT_BETWEEN(95, 105, pres);
}

void optical_measurement_test(void) {
    for (uint8_t field_num = 0; field_num < CAN_PAY_OPT_FIELD_COUNT; field_num++) {
        uint32_t data = read_opt_spi(field_num);
        ASSERT_BETWEEN(0, 0xFFFFFF, data);
    }
}

void thermistor_test(void) {
    uint8_t adc_channels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    uint8_t adc_channels_len = 12;

    fetch_all_adc_channels(&adc);

    for (uint8_t i = 0; i < adc_channels_len; i++) {
        uint8_t channel = adc_channels[i];
        uint16_t raw_data = read_adc_channel(&adc, channel);
        double voltage = adc_raw_data_to_raw_vol(raw_data);
        double resistance = therm_vol_to_res(voltage);
        double temp = therm_res_to_temp(resistance);

        ASSERT_BETWEEN(30, 40, temp);
    }
}

test_t t1 = { .name = "temperature sensor test", .fn = temperature_test };
test_t t2 = { .name = "humidity sensor test", .fn = humidity_test };
test_t t3 = { .name = "pressure sensor test", .fn = pressure_test };
test_t t4 = { .name = "optical sensor test", .fn = optical_measurement_test };
test_t t5 = { .name = "thermistor test", .fn = thermistor_test };

test_t* suite[] = { &t1, &t2, &t3 , &t4, &t5};

int main(void) {
    init_uart();
    init_spi();

    init_temp();
    init_hum();
    init_pres();

    init_opt_spi();
    rst_opt_spi(); //TODO: Have a test for the reset too

    init_adc(&adc);

    run_tests(suite, 5);
    return 0;
}
