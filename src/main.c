#include "main.h"



int main (void){
	init_spi();
	init_uart();
	sensor_setup();
	print("\n\n\nInitialized\n");

	init_adc();
	set_PGA(1);

	int adc_channel = 5;

	while (1) {
		select_ADC_channel(adc_channel);

		double AIN = read_ADC_channel(adc_channel);
		print("AIN = %ld%%\n", (uint32_t) (AIN / V_REF * 100.0));

		_delay_ms(2000);

		// Cycle between channels 5-7
		if (adc_channel == 7) {
			adc_channel = 5;
		} else {
			adc_channel++;
		}
	}
}

void adc_sequence(){
	// set the PEX_RST pin to output high
	DDRC |= _BV(0);
	PORTC |= _BV(0);

	init_adc();
	write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);
}
