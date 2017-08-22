/*
Author: Shimi Smith

Code for sending and recievign can messages
-------------------------------------------

How to send a can message
-------------------------
- include can.h
- call can_init(0)
- create and fill an array of uint8_t - at most 8 elements in the array
- call can_send_message with the array, size of the array and an id

How to recieve
--------------
- program a recieve mob with init_rxx
- id of recieve mob needs to match id of sent message
- after recieved RXOK flag set

Things to add and/or change - I'll get started on these in the near future
---------------------------
- can_send_message and init_rx_mob should take in the data array
- make all data arrays 8 bytes long and just specify a variable length
- should probably have a struct defined for each mob - this will make stuff nicer
- init_rx_interrupts should take in the mob number - defaulting to mob 0 was just for testing purposes

*/

#include "can.h"
#include "uart.h"
#include "log.h"

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

uint8_t recieved_data[2];
st_cmd_t rx_mob;

void init_rx_mob(uint8_t size, uint8_t id){

	rx_mob.pt_data = &recieved_data[0];
	rx_mob.status = 0;  // clear status

	rx_mob.id.std = id;  // only accepts frames from this id
	rx_mob.ctrl.ide = 0; // This message object accepts only standard (2.0A) CAN frames
	rx_mob.ctrl.rtr = 0; // this message object is not requesting a remote node to transmit data back
	rx_mob.dlc = size; // Number of bytes (8 max) of data to expect
	rx_mob.cmd = CMD_RX_DATA; // assign this as a "Receive Standard (2.0A) CAN frame" message object

	while(can_cmd(&rx_mob) != CAN_CMD_ACCEPTED);  // Wait for MOb to configure (Must re-configure MOb for every transaction)

}


void init_rx_interrupts(){

	CANGIE |= _BV(ENIT) | _BV(ENRX);  // enable CAN interrupts and enable the recieve interrupt
	CANIE2 |= _BV(IEMOB0);

	sei();  // enable global interrupts
}

ISR(CAN_INT_vect){
	print("INTERRUPT\n");
	if(CANSTMOB & _BV(RXOK) && can_get_status(&rx_mob) == CAN_STATUS_COMPLETED){  // interrupt caused by recieve finished
		CANSTMOB &= ~_BV(RXOK);  // clear interrupt flag

		print("------------\n");
		print("%u\n", recieved_data[0]);
		print("%u\n", recieved_data[1]);
		print("------------\n");

		for(uint8_t i=0; i<2; i++) {recieved_data[i] = 0;}  // clear data array

		while(can_cmd(&rx_mob) != CAN_CMD_ACCEPTED);  // Wait for MOb to configure (Must re-configure MOb for every transaction)
	}
}