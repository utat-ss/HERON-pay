#include "can.h"
#include "uart.h"
#include "log.h"

int main(void){

	init_uart();  // tends to be useful for debugging


	can_init(0);  // intializes can


	 uint8_t data[2];  // I'm just using an array of size 2 for testing make it whatever you want aslong as it's <= 8

	// Transmiting Code
    while(1){

    	// load in some data
		data[0] = 0x2A;
		data[1] = 0x2A;

		can_send_message(data, 2, 0x08);  // can use any id. 
		//We will be using ids like this in the real code - (PAY_TX | OBC_RX | HK_DATA)


    }

    // Recieving code for other microcontroller
    uint8_t data[8];  // set up an array to recieve data
	init_rx_mob(&rx_mob, data, 2, 0x08);  // initialize an rx mob
	init_rx_interrupts(rx_mob);  // initializes interrupts

}
