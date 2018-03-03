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


// CAN ID ALLOCATION FOR PAYLOAD
void status_rx_callback(uint8_t* data, uint8_t len);
void status_tx_callback(uint8_t* data, uint8_t* len);
void cmd_tx_callback(uint8_t* data, uint8_t* len);
void cmd_rx_callback(uint8_t* data, uint8_t len);
void data_tx_callback(uint8_t* data, uint8_t* len);

/*
mob_t status_rx_mob = {
	.mob_num = 0,
	.mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = PAY_STATUS_RX_MOB_ID,
	.id_mask = CAN_RX_MASK_ID,
    .ctrl = default_rx_ctrl,

    .rx_cb = status_rx_callback
};

mob_t status_tx_mob = {
    .mob_num = 1,
	.mob_type = TX_MOB,
    .id_tag = PAY_STATUS_TX_MOB_ID,
    .ctrl = default_tx_ctrl,

    .tx_data_cb = status_tx_callback
};

mob_t cmd_tx_mob = {
	.mob_num = 2,
	.mob_type = TX_MOB,
	.id_tag = PAY_CMD_TX_MOB_ID,
	.ctrl = default_tx_ctrl,

	.tx_data_cb = cmd_tx_callback
};
*/

mob_t cmd_rx_mob = {
	.mob_num = 3,
	.mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = PAY_CMD_RX_MOB_ID,
	.id_mask = CAN_RX_MASK_ID,
    .ctrl = default_rx_ctrl,

    .rx_cb = cmd_rx_callback
};

mob_t data_tx_mob = {
    .mob_num = 5,
	.mob_type = TX_MOB,
    .id_tag = PAY_DATA_TX_MOB_ID,
    .ctrl = default_tx_ctrl,

    .tx_data_cb = data_tx_callback
};
