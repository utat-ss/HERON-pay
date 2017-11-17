/*

	FILENAME: 			main.c
	DEPENDENCIES:		adc, pex, spi, uart

	DESCRIPTION:		Main functions to be implemented on the PAY 32M1
	AUTHORS:				Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
	DATE MODIFIED:	2017-11-16
	NOTE:
								* Please adhere to the 'separate function for testing' rule
	BUG:
								* Compiling throws a warning that F_CPU has been previously defined
									by a file associated with CAN. Can #CDH sort this out?

	REVISION HISTORY:

		2017-11-16: 	Created header. Implemented 'separate function for testing' rule

*/


#include "main.h"


int main (void){
	init_uart();
	print("\nUART Initialized\n");

	// CALL YOUR SPECIFIC TESTING FUNCTIONS HERE
	adc_sequence();

}

void adc_sequence(void){
	// Code to be called for testing the ADC
	// Temporary solution to the frequent changes to main.c across channels
	int pga_gain = 1;
	uint32_t read_data;
	double converted_data;

	init_port_expander();
	init_adc();

	print("RUNNING ADC TESTING CODE");

	// BUG: See the comment in init_adc()
	// 			This should be called in init_adc() in final implementation
	write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);

	set_PGA(pga_gain);

	int channel = 0;
	while(1){

		read_data = read_ADC_channel(channel + 5);
		print("\nRead Data (Channel %d)\n", channel + 5);
		print("Data = %lX (HEX) = %lu (DEC)\n", read_data, read_data);
		print("Gain = %d\n", pga_gain);

		converted_data = convert_ADC_reading(read_data, pga_gain);

		print("Converted voltage reading: %u mV", (uint16_t)(converted_data * 1000));

		// Count 0, 1, 2
		// Adding 5 gives channels 5, 6, 7, the only three hooked up
		channel = (channel + 1) % 3;

		_delay_ms(1000);
	}
}
