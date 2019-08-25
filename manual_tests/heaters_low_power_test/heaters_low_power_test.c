/*
Test heaters low power mode
*/

#include <uart/uart.h>

#include "../../src/general.h"
#include "../../src/heaters.h"


uint8_t count = 0;
// Counts down for wait
void heaters_countdown(void) {
    print("Heater's timer: %d\n", count);
    if (count > 0) {
        count -= 1;
    }
}

int main(void) {
    init_uart();
    init_dac(&dac);
    init_adc(&adc);

    init_uptime();
    init_heaters();

    print("\n\n\nStarting test\n\n");

    // Turn heaters on
    set_heaters_1_to_4_temp_setpoint(100);
    set_heater_5_temp_setpoint(100);

    add_uptime_callback(heaters_countdown);

    while (1) {
        print("\nLow power mode off\n");
        count = 10;
        _delay_ms(count * 1000);
        print("\nStarting low power mode\n");
        start_low_power_mode();
        count = HEATER_LOW_POWER_TIMER;
        _delay_ms(count * 1000);
    }

    return 0;
}
