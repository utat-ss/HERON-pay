#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "pex.h"
#include "spi.h"
#include "uart.h"
#include "log.h"

#define HUM_CS 0
#define PRES_CS 1
#define TEMP_CS 2

uint32_t getHumidityAndTemperature();
