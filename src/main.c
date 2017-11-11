#include "main.h"

int main (void){
	// This main.c file shoud be replaced with the one from origin/master before
	// creating a pull request.
	//
	// This code is only meant for testing purposes.

	sensor_setup();
	init_uart();
	print("\n");
	print("UART Initialized\n");
	print("Running humidity debug code\n");

	// Set the LEDs on the LED PCB to output

	while(1){
		print("Humidity:\t%d\n", (int)(read_humidity() * 100));
		_delay_ms(100);
	}
}
