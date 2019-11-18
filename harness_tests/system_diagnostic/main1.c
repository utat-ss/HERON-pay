#include <stdlib.h>
#include <test/test.h>

#include "../../src/general.h"

#define TEST_SIZE 2

/* Resets the OBC SSM and verifies that the reset counter increments correctly */
void obc_reset_test(void){
    uint32_t stale_num_restarts = 0x00;

    hb_send_obc_req = true;
    run_hb();
    stale_num_restarts = hb_latest_restart_count;

    send_hb_reset(HB_OBC);

    _delay_ms(1000);
    hb_send_obc_req = true;
    run_hb();

    ASSERT_EQ(hb_latest_restart_count, stale_num_restarts + 1);
    ASSERT_EQ(hb_latest_restart_reason, UPTIME_RESTART_REASON_EXTRF);
}

/* Resets the EPS SSM and verifies that the reset counter increments correctly */
void eps_reset_test(void){
    uint32_t stale_num_restarts = 0x00;

    hb_send_eps_req = true;
    run_hb();
    stale_num_restarts = hb_latest_restart_count;

    send_hb_reset(HB_EPS);

    _delay_ms(1000);
    hb_send_eps_req = true;
    run_hb();

    ASSERT_EQ(hb_latest_restart_count, stale_num_restarts + 1);
    ASSERT_EQ(hb_latest_restart_reason, UPTIME_RESTART_REASON_EXTRF);
}

test_t t1 = { .name = "OBC Reset Test", .fn = obc_reset_test };
test_t t2 = { .name = "EPS Reset Test", .fn = eps_reset_test };

test_t* suite[] = { &t1, &t2 };

int main(void) {
    init_pay();
    init_hb(HB_PAY);
    run_tests(suite, TEST_SIZE);
    return 0;
}
