/*
Author: Shimi Smith
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 8000000
#endif

#define PRESCALER 1024.0
#define T ((PRESCALER / F_CPU) * 1000.0)  // timer clock period (ms)
#define MAX_TIME (T * 0xFFFF)  // the maximum time the 16 bit timer can hold until it overflows (ms)

#define ROUND 0.5  // used to round double to int

typedef void (*Command)(void);

// This struct holds important variables for the timer
typedef struct Timer{
	uint8_t ints;  // the number of interrupts that will occur to achieve the desired time
	uint16_t count;  // the value of the timer counter after the desired time has ellapsed
	Command cmd;  // The command to run once the desired time has passed
}Timer;