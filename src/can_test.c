#include "can.h"
#include "uart.h"
#include "log.h"

int main(void){	

	init_uart();


	can_init(0);
	

	// Transmiting Code

	// uint8_t data[2];
	
 //    while(1){
	
	// 	data[0] = 0x2A;
	// 	data[1] = 0x2A;

	// 	can_send_message(data, 2, 0x08);  // can use any id

		
 //    }

    // Recieving code for other microcontroller
    uint8_t data[8];
	init_rx_mob(&rx_mob, data, 2, 0x08);
	init_rx_interrupts(rx_mob);

	while(1){
	}

}



