/*
Author: Shimi Smith

Code for sending and recieving can messages
*/

#include "can.h"
#include "uart.h"
#include "log.h"

/*
Sends a can message of maximum 8 bytes
--------------------------------------
data - an array of the data being sent
size - the number of bytes being sent (maximum 8)
id - the id of the can message
*/
void can_send_message(uint8_t data[], uint8_t size, uint8_t id){

	st_cmd_t message;  // message object

	message.pt_data = &data[0];  // point message object to first element of data buffer
	message.ctrl.ide = 0;  // standard CAN frame type (2.0A)
	message.dlc = size;  // Number of bytes being sent (8 max)
	message.id.std = id;  // populate ID field with ID Tag
	message.cmd = CMD_TX_DATA;  // assign this as a transmitting message object.
	
	while(can_cmd(&message) != CAN_CMD_ACCEPTED);  // wait for MOb to configure
	
	while(can_get_status(&message) == CAN_STATUS_NOT_COMPLETED);  // wait for message to send or fail

}

/*
Initializes a mob for recieving can messages
--------------------------------------------
mob - a pointer to the struct that is representing the mob that is being initialized
recieved_data[] - an array that will hold the data recieved
size - the number of bytes the mob will recieve (maximum 8)
id - this mob is programmmed to recieve can messages with this id, i.e this id must be the same as the id of the can messages you wish to recieve
*/
void init_rx_mob(st_cmd_t* mob, uint8_t recieved_data[], uint8_t size, uint8_t id){

	mob->pt_data = recieved_data;
	mob->status = 0;  // clear status

	mob->id.std = id;  // only accepts frames from this id
	mob->ctrl.ide = 0; // This message object accepts only standard (2.0A) CAN frames
	mob->ctrl.rtr = 0; // this message object is not requesting a remote node to transmit data back
	mob->dlc = size; // Number of bytes (8 max) of data to expect
	mob->cmd = CMD_RX_DATA; // assign this as a "Receive Standard (2.0A) CAN frame" message object

	while(can_cmd(mob) != CAN_CMD_ACCEPTED);  // Wait for MOb to configure (Must re-configure MOb for every transaction)

}

/*
Enables the can recieve interrupt for the given mob
---------------------------------------------------
precondition - mob was already initialized as a recieving mob with init_rx_mob

mob - the mob to initialize interrupts on
*/
void init_rx_interrupts(st_cmd_t mob){

	CANGIE |= _BV(ENIT) | _BV(ENRX);  // enable CAN interrupts and enable the recieve interrupt
	CANIE2 |= _BV(mob.handle);

	sei();  // enable global interrupts
}

/*
This is the ISR to handle all can interrupts
--------------------------------------------
*/
ISR(CAN_INT_vect){
	if(CANSTMOB & _BV(RXOK)){  // interrupt caused by recieve finished

		if(can_get_status(&rx_mob) == CAN_STATUS_COMPLETED){  // interrupt on rx_mob

			CANIE2 &= ~_BV(rx_mob.handle);  // disable interrupts for this mob - this is in case the mob number changes

			CANSTMOB &= ~_BV(RXOK);  // clear interrupt flag

			// do something with data or just print it
			print("------------\n");
			for(uint8_t i = 0; i < rx_mob.dlc; i++){
				print("%u\n", rx_mob.pt_data[i]);
			}
			print("------------\n");

			for(uint8_t i=0; i < rx_mob.dlc; i++) {rx_mob.pt_data[i] = 0;}  // clear data array

			while(can_cmd(&rx_mob) != CAN_CMD_ACCEPTED);  // Wait for MOb to configure (Must re-configure MOb for every transaction)

			CANIE2 |= _BV(rx_mob.handle);  // re-enable the interrupt - might be a new mob number
		}
	}
}