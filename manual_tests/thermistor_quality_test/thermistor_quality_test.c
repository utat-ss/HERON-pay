/*
Test made for the thermal subsystem according to the "Thermistor Quality
Assessment Overview" document.

We want to take measurements as fast as possible as consistent intervals, so:
- Use a timer interrupt to keep track of the elapsed time. In every iteration of
  the main loop, block until the time interval has elapsed.
- Don't use any atomic blocks because we do not want to delay the timer
  interrupt from occurring for counting.
- Don't use print statements inside the timer interrupt callback
  - To not delay it
  - To not output garbage in the middle of another print statement in the main
    loop.

Thermistor 1 corresponds to ADC channel 0, thermistor 2 to channel 1, etc.
*/

#include "stdbool.h"

#include <adc/adc.h>
#include <conversions/conversions.h>
#include <spi/spi.h>
#include <timer/timer.h>
#include <uart/uart.h>

#include "../../src/devices.h"


// NOTE: need to increase if you get the warning (see below)
#define DATA_PERIOD_MS 200UL   // in ms


// NOTE: kind of hacky, copied start_timer_8bit() from timer.c and modified
// the first line because we need millisecond precision

// Need visible declaration
extern timer_t timer8;

void start_timer_8bit_ms(uint16_t milliseconds, timer_fn_t cmd) {
    // delay in ms
    uint32_t delay = milliseconds;
    // number of interrupts
    timer8.max_time_ints = delay / MAX_TIME_8BIT;
    uint32_t remaining_delay = delay - (timer8.max_time_ints * MAX_TIME_8BIT);
    //remaining counter value
    timer8.remainder_time = (remaining_delay / PERIOD) + ROUND;

    //command to run
    timer8.cmd = cmd;

    // Update timer registers atomically so we don't accidentally trigger an
    // interrupt
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Clear any pending interrupt flags (if we started and stopped the
        // timer before) (p. 149)
        TIFR0 |= _BV(OCF0A);

        // set timer to CTC mode - using OCR0A
        TCCR0A &= ~_BV(WGM00);
        TCCR0A |= _BV(WGM01);
        TCCR0B &= ~_BV(WGM02);

        // set timer to use internal clock with prescaler of 1024
        TCCR0B |= _BV(CS02) | _BV(CS00);
        TCCR0B &= ~_BV(CS01);

        // disable use of output compare pins so that they can be used as normal pins
        TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0));

        // initialize 8 bit counter at 0
        TCNT0 = 0;

        // set compare value
        if (timer8.max_time_ints == 0) {
            OCR0A = timer8.remainder_time;
        } else {
            OCR0A = 0xFF;
        }

        // enable output compare interupt
        TIMSK0 |= _BV(OCIE0A);
    }

    // enable global interrupts
    sei();
}


// Must be volatile
volatile bool period_done = false;
volatile uint32_t time_ms = 0;

// Timer callback function
void timer_cb(void) {
    // Can uncomment this print to see the timer interrupt is firing,
    // but DO NOT LEAVE IT IN WHEN RUNNING THE ACTUAL TEST
    // print("Timer callback\n");

    period_done = true;
    time_ms += DATA_PERIOD_MS;
}


int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_adc(&adc);
    print("ADC Initialized\n");

    start_timer_8bit_ms(DATA_PERIOD_MS, timer_cb);
    print("Timer initialized with period %u ms\n", DATA_PERIOD_MS);

    // Data header for CSV
    print("\nTime (ms)");
    for (uint8_t i = 1; i <= 10; i++) {
        print(", Resistance %u (kohms), Temperature %u (C)", i, i);
    }
    print("\n");


    print("\nStarting test\n\n");

    while (1) {
        if (period_done) {
            print("WARNING: Timer too fast, need to make DATA_PERIOD_MS (%u) larger\n", DATA_PERIOD_MS);
        }
        while (!period_done) {}
        period_done = false;

        print("%lu", time_ms);
        // Find resistance/temperature for each channel
        for (uint8_t channel = 0; channel < 10; channel++) {
            fetch_adc_channel(&adc, channel);
            uint16_t raw_data = read_adc_channel(&adc, channel);
            double voltage = adc_raw_to_ch_vol(raw_data);
            double resistance = therm_vol_to_res(voltage);
            double temp = therm_res_to_temp(resistance);
            // Print temperature/resistance/voltage of thermistors
            print(", %.3f , %.3f", resistance, temp);
        }
        print("\n");
    }
}
