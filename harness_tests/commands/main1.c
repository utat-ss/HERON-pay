#include <test/test.h>

#include <conversions/conversions.h>

#include "../../src/heaters.h"

// 2
void count_ones_test(void) {
    uint8_t array1[12] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    ASSERT_EQ(count_ones(array1, 12), 12);

    uint8_t array2[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    ASSERT_EQ(count_ones(array2, 12), 0);

    uint8_t array3[12] = { 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1 };
    ASSERT_EQ(count_ones(array3, 12), 5);
}

void enables_to_uint_test(void) {
    uint8_t array1[5] = { 1, 0, 0, 1, 1 };
    ASSERT_EQ(enables_to_uint(array1, 5), 0x19);

    uint8_t array2[12] = { 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1 };
    ASSERT_EQ(enables_to_uint(array2, 12), 0xF09);
}

void default_values_test(void) {
    ASSERT_FP_EQ(adc_raw_to_therm_temp(HEATERS_SETPOINT_RAW_DEFAULT), 13.975);
    ASSERT_FP_EQ(adc_raw_to_therm_temp(INVALID_THERM_READING_RAW_DEFAULT), 19.988);

    // Test possible raw values to use
    // print("0x%x\n", adc_ch_vol_to_raw(therm_res_to_vol(therm_temp_to_res(10))));
    // print("0x%x\n", adc_ch_vol_to_raw(therm_res_to_vol(therm_temp_to_res(14))));
    // print("0x%x\n", adc_ch_vol_to_raw(therm_res_to_vol(therm_temp_to_res(20))));
}

test_t t1 = { .name = "count_ones_test", .fn = count_ones_test };
test_t t2 = { .name = "enables_to_uint_test", .fn = enables_to_uint_test };
test_t t3 = { .name = "default_values_test", .fn = default_values_test };

test_t* suite[] = { &t1, &t2, &t3 };

int main(void) {
    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    return 0;
}
