#include "main.h"

void adc_sequence(void);

int main (void){
	int i;
	// This main.c file shoud be replaced with the one from origin/master before
	// creating a pull request.
	//
	// This code is only meant for testing purposes.

	init_port_expander();
	init_uart();
	print("\n");
	print("UART Initialized\n");
	print("Running ADC debug code\n");

	adc_sequence();
}

void adc_sequence(void){
	// set the PEX_RST pin to output high
	DDRC |= _BV(0);
	PORTC |= _BV(0);

	init_adc();
	write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);
}
