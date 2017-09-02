/* 
Author: Shimi Smith
-------------------
- This is code to read from, write to and toggle io pins
*/

#include "port.h"

void setPinMode(uint8_t pin, Port DDR, uint8_t mode){
	if(mode == INPUT){
		*DDR &= ~_BV(pin);  // write DDxn to 0 to make it an input pin
	}
	else if(mode == OUTPUT){
		*DDR |= _BV(pin);  // write DDxn to 1 to make it an output pin
	}
}

void write(uint8_t pin, Port PORT, uint8_t value){
	if(value == HIGH){
		*PORT |= _BV(pin);  // write PORTxn to 1 to write high
	}
	else if(value == LOW){
		*PORT &= ~_BV(pin);  // write PORTxn to 0 to write low
	}
}

void toggle(uint8_t pin, Port PORT){
	*PORT ^= _BV(pin);
}

uint8_t read(uint8_t pin, uint8_t PIN){
	return (PIN >> pin) & 0x01;
}