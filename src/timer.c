/* 
 Author: Shimi Smith
 This is a timer that runs a given function every x minutes
*/

#include "timer.h"

static Timer timer;

void set_vars(uint8_t minutes, Command cmd){
	uint32_t delay = minutes * 60L * 1000L;  // delay in ms
	timer.ints = ((delay / MAX_TIME));
	timer.count = ((delay - (timer.ints * MAX_TIME)) / T) + ROUND;
	timer.cmd = cmd;
}

void init_timer(uint8_t minutes, Command cmd){
	set_vars(minutes, cmd);

	// set timer to CTC mode - using OCR1A
	TCCR1A &= ~(_BV(WGM10) | _BV(WGM11));
	TCCR1B |= _BV(WGM12);
	TCCR1B &= ~_BV(WGM13);

	// set timer to use internal clock with prescaler of 1024
	TCCR1B |= _BV(CS12) | _BV(CS10);
	TCCR1B &= ~_BV(CS11);

	// disable use of Output compare pins so that they can be used as normal pins
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0));

	TCNT1 = 0;  // initialize counter at 0
	OCR1A = 0xFFFF;  // set compare value

	// enable output compare interupt
	TIMSK1 |= _BV(OCIE1A);
	sei(); // enable global interrupts
}

volatile uint32_t counter = 0;  // the number of interrupts that have occured
// This ISR occurs when TCNT1 is equal to OCR1A
ISR(TIMER1_COMPA_vect){

	counter++;

	if(counter == timer.ints){
		OCR1A = timer.count;
	}
	else if(counter >= timer.ints + 1){  // the desired time has passed
		counter = 0;
		OCR1A = 0xFFFF;

		(*(timer.cmd))();

	}
}



