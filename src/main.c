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

		2017-11-17: 	Added "sensor_led_sequence()" as a sanity check
		2017-11-16: 	Created header. Implemented 'separate function for testing' rule

*/


#include "main.h"


int main (void){
	init_uart();
	print("\n\nUART Initialized\n");
	// CALL YOUR SPECIFIC TESTING FUNCTIONS HERE
	//adc_test_sequence();
	//sensor_led_sequence();
	setup_adc();
	//poll_pht();
	poll_sfh();
	//poll_int();


}

/*void adc_test_sequence(void){
	// Code to be called for testing the ADC

	int pga_gain = 1;
	uint32_t read_data;
	uint32_t config_data;
	double converted_data;

	init_port_expander();
	init_adc();

	print("RUNNING ADC TESTING CODE\n");

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

		// In here for sanity, for the time being
		config_data = read_ADC_register(CONFIG_ADDR);
		print("Configuration Register: %lX\n", config_data);

		converted_data = convert_ADC_reading(read_data, pga_gain);

		print("Converted voltage reading: %lu mV\n", (uint32_t)(converted_data * 1000));

		// Count 0, 1, 2
		// Adding 5 gives channels 5, 6, 7, the only three hooked up
		channel = (channel + 1) % 3;

		_delay_ms(500);
	}
}*/

void sensor_led_sequence(void){
	// Simple function which cycles the LEDs on the sensor PCB
	// Good sanity check. If this works, then SPI and the PEX work

	int i = 0;

	print("Starting Sensor PCB LED Sequence");

	init_port_expander();

	// Set the four LED pins to output
	for (i = 0; i < 4; i++){
		set_dir_a(SENSOR_PCB, i, 0);
	}

	while(1){
		clear_gpio_a(SENSOR_PCB, i);

		i = (i + 1) % 4;

		set_gpio_a(SENSOR_PCB, i);
		_delay_ms(500);
	}
}

void setup_adc(void){
	int pga_gain = 1;

	init_port_expander();
	init_adc();
	print("ADC Setup Completed\n");

	write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);

	set_PGA(pga_gain);

	print("PGA gain: %d", pga_gain);

	print("Read Data (HEX)\tRead Data (DEC)\tConverted Voltage\n");
}

void poll_adpd(void){
	int channel = 7;
	int LED = LED_4;
	int i;

	print("Channel %d\n", channel);

	set_gpio_a(SENSOR_PCB, LED);
	_delay_ms(1);

	for (i=0; i<10; i++){
		uint32_t read_data = read_ADC_channel(channel, LED);
		print("%lX\t %lu\n", read_data, read_data);
	}

	clear_gpio_a(SENSOR_PCB, LED);
}

void poll_sfh(void){
	int channel = 6;
	int LED = LED_3;
	int i;

	print("Channel %d\n", channel);

	for (i=0; i<20; i++){
		uint32_t read_data = read_ADC_channel(channel, LED);
		print("%lX\t %lu\n", read_data, read_data);
	}

	//clear_gpio_a(SENSOR_PCB, LED);
}


void poll_pht(void){
	int channel = 5;
	int LED = LED_2;
	int i;

	print("Channel %d\n", channel);

	for (i=0; i<20; i++){
		uint32_t read_data = read_ADC_channel(channel, LED);
		print("%lX\t %lu\n", read_data, read_data);
	}

	//clear_gpio_a(SENSOR_PCB, LED);
}

void poll_int(void){
	set_dir_b(SENSOR_PCB, ITF_CS, 0);
	set_gpio_b(SENSOR_PCB, ITF_CS);
	init_freq_measure();

	int LED = LED_1;
	uint32_t read_data;
	float frequency;
	int i=0;

	print("Int to freq\n");
	//set_gpio_a(SENSOR_PCB, LED);
	_delay_ms(1);
	clear_gpio_b(SENSOR_PCB, ITF_CS);

	for (i=0;i<100;i++){
		while (!available()){
			continue;
		}
		read_data = read_freq_measure();
		frequency = convert_count_to_freq(read_data);

		print("%lu\n",frequency);
		_delay_ms(100);
	}
	set_gpio_b(SENSOR_PCB, ITF_CS);
	//clear_gpio_a(SENSOR_PCB, LED);

	end_freq_measure();
}
