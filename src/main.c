/**
This is WIP main code
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


int main (void){
	init_uart();
	print("Main has started\n");

	init_can();
	init_rx_mob(&rx_mob);

	Queue temp_queue = init_queue();
	cmd_queue = &temp_queue;

	while(1){
		if(!isEmpty(queue)){
			Data cmd = dequeue(queue);
			run_cmd(cmd);
		}
	}
}

void pay_rx_callback(uint8_t* data, uint8_t len) {
    Data cmd;
    cmd.array = data;
    enqueue(cmd_queue, cmd);
}

void run_cmd(Data cmd){
	print("%s%u\n", "Data: ", cmd);
}



