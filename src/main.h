// Definition necessary for <util/delay.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include <stdint.h>

// Core libraries from lib-common
#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <spi/spi.h>

// Pay-specific libraries
