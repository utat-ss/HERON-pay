#include <stdlib.h>
#include <test/test.h>

#include "../../src/general.h"

/* Resets the OBC SSM and verifies that the reset counter increments correctly */
void obc_reset_test(void){
    uint32_t stale_num_restarts = 0x00;

    obc_hb_dev.send_req_flag = true;
    _delay_ms(100);
    run_hb();
    _delay_ms(100);
    stale_num_restarts = obc_hb_dev.restart_count;

    send_hb_reset((hb_dev_t*) &obc_hb_dev);

    _delay_ms(1000);
    obc_hb_dev.send_req_flag = true;
    _delay_ms(100);
    run_hb();
    _delay_ms(100);

    ASSERT_EQ(obc_hb_dev.restart_count, stale_num_restarts + 1);
    ASSERT_EQ(obc_hb_dev.restart_reason, UPTIME_RESTART_REASON_EXTRF);
}

/* Resets the EPS SSM and verifies that the reset counter increments correctly */
void eps_reset_test(void){
    uint32_t stale_num_restarts = 0x00;

    eps_hb_dev.send_req_flag = true;
    _delay_ms(100);
    run_hb();
    _delay_ms(100);
    stale_num_restarts = eps_hb_dev.restart_count;

    send_hb_reset((hb_dev_t*) &eps_hb_dev);

    _delay_ms(1000);
    eps_hb_dev.send_req_flag = true;
    _delay_ms(100);
    run_hb();
    _delay_ms(100);

    ASSERT_EQ(eps_hb_dev.restart_count, stale_num_restarts + 1);
    ASSERT_EQ(eps_hb_dev.restart_reason, UPTIME_RESTART_REASON_EXTRF);
}

test_t t1 = { .name = "OBC Reset Test", .fn = obc_reset_test };
test_t t2 = { .name = "EPS Reset Test", .fn = eps_reset_test };

test_t* suite[] = { &t1, &t2 };

int main(void) {
    WDT_OFF();
    init_pay();
    init_hb(HB_PAY);
    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    return 0;
}
