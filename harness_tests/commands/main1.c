#include <test/test.h>

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


test_t t1 = { .name = "count_ones_test", .fn = count_ones_test };
test_t t2 = { .name = "enables_to_uint_test", .fn = enables_to_uint_test };

test_t* suite[] = { &t1, &t2 };

int main(void) {
    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    return 0;
}
