#include <can/can.h>
#include <uart/uart.h>
#include <uart/log.h>
#include "can_handler.h"

#define IS_SENDING 0
#define IS_MATCHING_ADDR 1

int main(void){
	init_uart();  // tends to be useful for debugging
	can_init(0);  // intializes can
	uint8_t data[2];  // I'm just using an array of size 2 for testing make it whatever you want aslong as it's <= 8
	
	if(IS_SENDING){
		while(1){
	    	// load in some data
			data[0] = 0x2A;
			data[1] = 0x2A;
			can_send_message(data, 2, 0x7a);  // can use any id. 
			//We will be using ids like this in the real code - (PAY_TX | OBC_RX | HK_DATA)
    	}
	} else {
	    set_can_handler(&pay_can_handler);
	    
	    if(IS_MATCHING_ADDR) {
			init_rx_mob(&rx_mob, data, 2, 0x7a);  // initialize an rx mob
	    } else {
			init_rx_mob(&rx_mob, data, 2, 0x70);
	    }

		init_rx_interrupts(rx_mob);  // initializes interrupts
		
		print("Hello team!\n");
		while(1){}
	}
}