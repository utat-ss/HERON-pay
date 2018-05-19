#include <uart/log.h>
#include <uart/uart.h>
#include <assert/assert.h>

int main(void){
    init_uart();
    print("UART initialized\n");

    ASSERT(1 == 2);
    ASSERT(3 > 2);
    ASSERT(5 <= 2);

    assert_print_results();
}
