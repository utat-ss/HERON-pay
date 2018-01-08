#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include <stdint.h>

// Core libraries from lib-common
#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>
#include <queue/queue.h>
#include <spi/spi.h>

// Pay-specific libraries
#include "sensors.h"
#include "adc.h"
#include "pex.h"
#include "freq_measure.h"
#include "analog_temp.h"

// CAN ID ALLOCATION FOR PAYLOAD
#define status_rx_mob_id	{ 0x000B }
#define status_tx_mob_id	{ 0x0012 }
#define cmd_tx_mob_id		{ 0x0022 }
#define cmd_rx_mob_id		{ 0x0043 }
#define data_tx_mob_id		{ 0x0102 }

#define can_rx_mask_id		{ 0x07F8 }

// [6] housekeeping / ~science
// [5:3]
#define pay_id_header 		0b10000000

void rx_callback(uint8_t* data, uint8_t len);
void tx_callback_1(uint8_t*, uint8_t*);
void tx_callback_2(uint8_t*, uint8_t*);
void run_cmd(Data cmd);
