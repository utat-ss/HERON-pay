#include "can.h"

int main(void){		

	can_init(0);
	
	uint8_t data[2];
	
    while(1){

		data[0] = 0xFF;
		data[1] = 0xFF;

		can_send_message(data, 2, 0x08);  // can use any id

		
    }
}


