#include <uart/log.h>
#include <uart/uart.h>

// #define ASSERT_DISABLE
#include <assert/assert.h>

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    // assert_print_on_pass = true;

    ASSERT(1 == 2);
    ASSERT(3 > 2);
    ASSERT(5 <= 2);

    assert_print_results();
}
