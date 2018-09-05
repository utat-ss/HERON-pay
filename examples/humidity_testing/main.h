#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include <stdint.h>

// Core libraries from lib-common
#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <can/can_ids.h>
#include <queue/queue.h>
#include <spi/spi.h>

// Pay-specific libraries
#include "sensors.h"
#include "adc.h"
#include "pex.h"
#include "freq_measure.h"
#include "analog_temp.h"
