#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "sensors.h"
#include <util/delay.h>

#include "uart.h"
#include "log.h"

#define TEMP_CS 1  // GPIOB register
#define LIGHT PB4

void setup(){
	port_expander_init();
	set_dir_b(0x00, 0x00);
	set_gpio_b(0x00, 0x00 | (1 << TEMP_CS));
	DDRB |= _BV(PB4);
	init_spi();
	init_uart();
}

int main (void)
{
	setup();
	while(1) {
		print("%d\n", (int)(readAndShutdown() * 100));
		//print("SANITY");
		_delay_ms(100);
	}
}
