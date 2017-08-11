/*
Author: Shimi Smith

Code for sending can messages
-----------------------------

To send a can message
	- include can.h
	- call can_init(0)
	- create and fill an array of uint8_t - at most 8 elements in the array
	- Call can_send_message with the array, size of the array and an id
*/

#include "can.h"

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