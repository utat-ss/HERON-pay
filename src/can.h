#include "can_lib.h"
// #include <"avr/interrupt.h">

void can_send_message(uint8_t data[], uint8_t size, uint8_t id);
void init_rx_mob(uint8_t size, uint8_t id);
void init_rx_interrupts();