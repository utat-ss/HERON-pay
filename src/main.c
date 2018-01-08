/*

	FILENAME: 			main.c
	DEPENDENCIES:		pex, uart, can, queue, freq_measure, stdint, delay

	DESCRIPTION:		Main functions to be implemented on the PAY 32M1
	AUTHORS:			Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
	DATE MODIFIED:		2017-12-28
	NOTE:
						PLEASE DON'T COMMIT YOUR TESTING CODE
						ONLY COMMIT ACTUAL CHANGES TO MAIN.C

	REVISION HISTORY:

        2018-01-08:     DV: Added CAN id definitions to the header file
		2017-11-20:		SS: Added testing for PAY command queue
		2017-11-17: 	Added "sensor_led_sequence()" as a sanity check
		2017-11-16: 	Created header. Implemented 'separate function for testing' rule

*/

#include "main.h"

// Initilaze the command queue as a global so it can be used in the CAN callback
// Queue cmd_queue;

mob_t status_rx_mob = {
	.mob_num = 0,
	.mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = status_rx_mob_id
	.id_mask = can_rx_mask_id
    .ctrl = default_rx_ctrl,

    .rx_cb = status_rx_callback
};

mob_t status_tx_mob = {
    .mob_num = 1,
	.mob_type = TX_MOB,
    .id_tag = status_tx_mob_id
    .ctrl = default_tx_ctrl,

    .tx_data_cb = status_tx_callback
};

mob_t cmd_tx_mob = {
	.mob_num = 2,
	.mob_type = TX_MOB,
	.id_tag = cmd_tx_mob_id,
	.ctrl = default_tx_ctrl,

	.tx_data_cb = cmd_tx_callback
};

mob_t cmd_rx_mob = {
	.mob_num = 3,
	.mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = cmd_rx_mob_id
	.id_mask = can_rx_mask_id
    .ctrl = default_rx_ctrl,

    .rx_cb = cmd_rx_callback
};

mob_t data_tx_mob = {
    .mob_num = 5,
	.mob_type = TX_MOB,
    .id_tag = data_tx_mob_id,
    .ctrl = default_tx_ctrl,

    .tx_data_cb = data_tx_callback
};

void pay_can_init(void){
	// TODO: Consider adding print statements which show the data loaded into
	// the TX MObs, for debugging.

	init_rx_mob(&status_rx_mob);
	init_tx_mob(&status_tx_mob);
	init_tx_mob(&cmd_tx_mob);
	init_rx_mob(&cmd_rx_mob);
	init_tx_mob(&data_tx_mob);
}

void status_rx_callback(uint8_t* data, uint8_t len){

	// Resume the MOb to send the stasus back
	resume_mob(&status_tx_mob);
}
void status_tx_callback(uint8_t* data, uint8_t* len){
	
}
void cmd_tx_callback(uint8_t* data, uint8_t* len){

}
void cmd_rx_callback(uint8_t* data, uint8_t len){

}
void data_tx_callback(uint8_t* data, uint8_t* len){
	// Callback specifically for sending housekeeping/sensor data

}

int main (void){
	init_uart();
	print("\n\nUART Initialized\n");
	pay_can_init();
	// cmd_queue = initQueue();
    init_can();

	// Testing bi-directional CAN transfers
	init_rx_mob(&rx_mob);
	// init_tx_mob(&tx_mob_1);
    // init_tx_mob(&tx_mob_2);
    //
    // resume_mob(&tx_mob_1);
    while(1){};
}

// currently just sending "Hello!"

/*
void tx_callback_1(uint8_t* data, uint8_t* len) {
    static uint32_t val_1 = 0;
    if (val_1 % 2 == 0) {
        data[0] = 0x01;
        *len = 1;
    } else {
        *len = 0;
        resume_mob(&tx_mob_2);
    }

    val_1 += 1;
}

void tx_callback_2(uint8_t* data, uint8_t* len) {
    static uint32_t val_2 = 0;
    if (val_2 % 2 == 0) {
        data[0] = 0x02;
        *len = 1;
    } else {
        *len = 0;
        resume_mob(&tx_mob_1);
    }
    val_2 += 1;
}

void rx_callback(uint8_t* data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++){
        print("Data: %u", data[i]);
    }
    print("\n");
}*/

void run_cmd(Data cmd){
	// Should implement a large switch-case statement to handle different commands
	print("\n%s\n\n", (char *) cmd.array);
}
