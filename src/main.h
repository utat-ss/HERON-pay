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

Queue* cmd_queue;

//void pay_rx_callback(uint8_t* data, uint8_t len);
//void tx_callback(uint8_t*, uint8_t*);
//void run_cmd(Data cmd);
/*
rx_mob_t rx_mob = {
    .mob_num = 0,
    .dlc = 7,
    .id_tag = 0x0000,
    .id_mask = 0xFFFF,
    .ctrl = default_rx_ctrl,
    .rx_cb = pay_rx_callback
};

// this is a tx mob used for testing
tx_mob_t tx_mob = {
    .mob_num = 0,
    .id_tag = { 0x0000 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};*/
