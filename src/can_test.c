#include "can.h"
#include "uart.h"
#include "log.h"

int main(void){	

	init_uart();


	can_init(0);
	
	// uint8_t data[2];
	
 //    while(1){

	// 	data[0] = 0x2A;
	// 	data[1] = 0x2A;

	// 	can_send_message(data, 2, 0x08);  // can use any id

		
 //    }

    //recieving code for other microcontroller
	init_rx_interrupts();
	init_rx_mob(2, 0x08);

	while(1){
	}

}



