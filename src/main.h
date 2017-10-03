#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "sensors.h"
#include <util/delay.h>
#include <uart/uart.h>
#include <uart/log.h>

#include "queue.h"
#include "can.h"
