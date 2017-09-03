#include "can_lib.h"

void can_send_message(uint8_t data[], uint8_t size, uint8_t id);
void init_rx_mob(st_cmd_t* mob, uint8_t recieved_data[], uint8_t size, uint8_t id);
void init_rx_interrupts(st_cmd_t mob);

st_cmd_t rx_mob;