/**
Author: Shimi Smith
**/

#include "main.h"

rx_mob_t rx_mob = {
    .mob_num = 0,
    .dlc = 7,
    .id_tag = 0x0000,
    .id_mask = 0xFFFF,
    .ctrl = default_rx_ctrl,
    .rx_cb = pay_rx_callback
};

// this is a tx mob used for testing
tx_mob_t tx_mob = {
    .mob_num = 0,
    .id_tag = { 0x0000 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};


int main (void){
	init_uart();

	init_can();

	init_rx_mob(&rx_mob);

	Queue temp_queue = initQueue();
	cmd_queue = &temp_queue;

	while(1){
		if(!isEmpty(cmd_queue)){
			Data cmd = dequeue(cmd_queue);
			run_cmd(cmd);
		}
	}

	// This is how we send can messages
	// init_tx_mob(&tx_mob);
	// resume_tx_mob(&tx_mob);
	// while(1){}
}

// currently just sending "Hello!"
void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 7;

    char str[] = "Hello!";

    for(uint8_t i = 0; i < *len; i++) {
        data[i] = str[i];
    }
}

void pay_rx_callback(uint8_t* data, uint8_t len) {

	// create a Data struct from the recieved data
    Data cmd;
    for (uint8_t i = 0; i < len; i++){
    	cmd.array[i] = data[i];
    }

    enqueue(cmd_queue, cmd);  // add command into queue
}


void run_cmd(Data cmd){
	print("%s\n", (char *) cmd.array);
}
