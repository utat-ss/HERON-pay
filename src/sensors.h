#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"
#include "pex.h"

#define TEMP_CS 1  // Located on GPIOB1
float temperature_read(void);
