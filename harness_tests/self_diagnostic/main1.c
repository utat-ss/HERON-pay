// Standard libraries
#include <stdlib.h>

// lib-common libraries
#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <test/test.h>
#include <can/data_protocol.h>

// PAY libraries
#include "../../src/devices.h"
#include "../../src/env_sensors.h"
#include "../../src/optical_spi.h"

#define ASSERT_BETWEEN(least, greatest, value) \
    ASSERT_GREATER(value, least); \
    ASSERT_LESS(value, greatest);

void temperature_test(void) {
    uint16_t temp_raw = read_temp_raw_data();
    double temp = temp_raw_data_to_temperature(temp_raw);
    // ASSERT_BETWEEN(25, 37, (uint8_t) temp);
    ASSERT_BETWEEN(20, 30, (uint8_t) temp);
}

void humidity_test(void) {
    uint16_t hum_raw = read_hum_raw_data();
    double hum = hum_raw_data_to_humidity(hum_raw);
    // ASSERT_BETWEEN(1, 50, (uint8_t) hum);
    ASSERT_BETWEEN(50, 60, (uint8_t) hum);
}

void pressure_test(void) {
    uint32_t pres_raw = read_pres_raw_data();
    double pres = pres_raw_data_to_pressure(pres_raw);
    ASSERT_BETWEEN(95, 105, (int16_t) pres);
}

void optical_measurement_test(void) {
    for (uint8_t field_num = 0; field_num < CAN_PAY_OPT_FIELD_COUNT; field_num++) {
        uint32_t data = read_opt_spi(field_num);
        ASSERT_NEQ(0, data);
        ASSERT_NEQ(0xFFFF, data);
    }
}

test_t t1 = { .name = "temperature sensor test", .fn = temperature_test };
test_t t2 = { .name = "humidity sensor test", .fn = humidity_test };
test_t t3 = { .name = "pressure sensor test", .fn = pressure_test };
test_t t4 = { .name = "optical sensor test", .fn = optical_measurement_test };

test_t* suite[] = { &t1, &t2, &t3 , &t4};

int main(void) {
    init_uart();
    init_spi();

    init_temp();
    init_hum();
    init_pres();

    init_opt_spi();
    rst_opt_spi(); //TODO: Have a test for the reset too

    run_tests(suite, 4);
    return 0;
}
