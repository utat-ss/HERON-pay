/* Author: Shimi Smith
----------------------
Sends CAN messages
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "can_lib.h"


void sys_init(void);
void io_init(void);
void can_send_message(uint8_t data[], uint8_t size, uint8_t id);


int main(void){		

	sys_init();
	
	uint8_t data[2];
	
    while(1){

		data[0] = 0xFF;
		data[1] = 0xFF;

		can_send_message(data, 2, 0x08);  // can use any id

		
    }
}

void can_send_message(uint8_t data[], uint8_t size, uint8_t id){  // Sends a maximum of 8

	st_cmd_t message;  // message object

	message.pt_data = &data[0];  // point message object to first element of data buffer
	message.ctrl.ide = 0;  // standard CAN frame type (2.0A)
	message.dlc = size;  // Number of bytes being sent (8 max)
	message.id.std = id;  // populate ID field with ID Tag
	message.cmd = CMD_TX_DATA;  // assign this as a transmitting message object.
	
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);  // wait for MOb to configure
	
	while(can_get_status(&message) == CAN_STATUS_NOT_COMPLETED);  // wait for message to send or fail

}

void sys_init(void) {
	// Make sure sys clock is at least 8MHz  -  I don't think we actually need to do this for can, I'll get rid once I confirm that
	CLKPR = 0x80;  
	CLKPR = 0x00;

	can_init(0);

}
