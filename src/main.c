/*

	FILENAME: 			main.c
	DEPENDENCIES:		adc, pex, spi, uart

	DESCRIPTION:		Main functions to be implemented on the PAY 32M1
	AUTHORS:				Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
	DATE MODIFIED:	2017-12-28
	NOTE:
								* Please adhere to the 'separate function for testing' rule
	BUG:
								* Compiling throws a warning that F_CPU has been previously defined
									by a file associated with CAN. Can #CDH sort this out?

	REVISION HISTORY:

		2017-11-20:		SS: Added testing for PAY command queue
		2017-11-17: 	Added "sensor_led_sequence()" as a sanity check
		2017-11-16: 	Created header. Implemented 'separate function for testing' rule

*/

#include "main.h"

// RX and TX mob now declared in main.h

int main (void){
	init_uart();
	print("\n\nUART Initialized\n");
	/*
    init_can();

	// CALL YOUR SPECIFIC TESTING FUNCTIONS HERE
    init_rx_mob(&rx_mob);

    Queue temp_queue = initQueue();
    cmd_queue = &temp_queue;

    while(1){
        if(!isEmpty(cmd_queue)){
            Data cmd = dequeue(cmd_queue);
            run_cmd(cmd);
        }
    }

    // This is how we send can messages
	// init_tx_mob(&tx_mob);
	// resume_tx_mob(&tx_mob);
	// while(1){}*/
}

// currently just sending "Hello!"
/*
void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 7;

    char str[] = "Hello!";

    for(uint8_t i = 0; i < *len; i++) {
        data[i] = str[i];
    }
}

void pay_rx_callback(uint8_t* data, uint8_t len) {

	// create a Data struct from the recieved data
    Data cmd;
    for (uint8_t i = 0; i < len; i++){
    	cmd.array[i] = data[i];
    }

    enqueue(cmd_queue, cmd);  // add command into queue
}


void run_cmd(Data cmd){
	print("%s\n", (char *) cmd.array);
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
