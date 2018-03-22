/*

    DESCRIPTION:    Blinks the LED connected to the 32M1 and the port expander
                    on PAY-1-04 V2 (PAY SSM V2)
    MAINTAINERS:    Dylan Vogel
    DATE MODIFIED:  2018-02-24

*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <spi/spi.h>
#include <util/delay.h>
#include "../../src/pex.h"

// 32M1 LED PIN
#define LED_PIN     PB4
#define LED_DDR     DDRB
#define LED_PORT    PORTB

// PORT EXAPNDER PIN
// led is on port B
#define PEX_LED 6

int main(void){
    // 32M1 SETUP
    // set the direction of the 32M1 pin to output (1)
    LED_DDR |= _BV(LED_PIN);
    // clear the pin
    LED_PORT &= ~(_BV(LED_PIN));

    // PORT EXPANDER SETUP
    init_port_expander();
    // set the direction of the port expander pin to output (0)
    set_dir_b(SSM, PEX_LED, 0);
    // clear the pin
    clear_gpio_b(SSM, PEX_LED);

    while(1){
        // turn both LEDs ON
        LED_PORT |= _BV(LED_PIN);
        set_gpio_b(SSM, PEX_LED);
        _delay_ms(1000);

        // turn both LEDs OFF
        LED_PORT &= ~(_BV(LED_PIN));
        clear_gpio_b(SSM, PEX_LED);
        _delay_ms(1000);
    }
}
