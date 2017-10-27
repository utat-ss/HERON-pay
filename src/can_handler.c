#include "can_handler.h"

void pay_can_handler (uint16_t identifier, uint8_t* pt_data, uint8_t size) {
	print("-----------\n");
	for(uint8_t i = 0; i< size; i++){
		print("%u\n", pt_data[i]);
	}
	print("-----------\n");
}