#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "uart.h"

// TODO fix
#define CS PD4
#define CS_PORT PORTD
#define CS_DDR DDRD

uint32_t getHumidityAndTemperature();
