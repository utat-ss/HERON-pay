#include "main.h"

/*void setup(){
	port_expander_init();
	set_dir_a(0, 0x00)
	set_dir_b(0, 0x00);
	set_gpio_b(0x00, 0x00 | (1 << TEMP_CS));
	DDRB |= _BV(PB4);
	init_spi();
	init_uart();
}*/

int main (void)
{
	//float temperature;
	sensor_setup();
	init_uart();
	print("\n");
	print("UART Initialized\n");
	set_dir_b(SSM, PEX_LED, 0);

	while(1){
		set_gpio_b(SSM, PEX_LED);
		_delay_ms(1000);
		clear_gpio_b(SSM, PEX_LED);
		_delay_ms(1000);

		print("Temperature:\t%d\n", ((int)(read_temperature() * 100)));
		print("Humidity:\t%d\n", (int)(read_humidity() * 100));
		_delay_ms(100);
		print("Temperature:\t%d\n", (int)(read_humidity_temp() * 100));
		// set_gpio_b(SSM, PEX_LED);
		// _delay_ms(1000);
		// clear_gpio_b(SSM, PEX_LED);
		// _delay_ms(1000);
	}
	/*setup();
	while(1) {
		print("%d\n", (int)(readAndShutdown() * 100));
		//print("SANITY");
		_delay_ms(100);
	}*/
	return 1;
}
