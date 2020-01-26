/*
Defines the physical interface (MOBs and callback functions) that PAY uses for
CAN communication.
*/

#include "can_interface.h"


/* Callback functions */

// MOB 3
// CMD RX - received commands
void cmd_rx_callback(const uint8_t* data, uint8_t len) {
    if (len == 0) {
        return;
    }

    // Add it to the queue of received messages to process
    enqueue(&rx_msg_queue, (uint8_t*) data);
}

// MOB 5
// Data TX - transmitting data
void data_tx_callback(uint8_t* data, uint8_t* len) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&tx_msg_queue)) {
            *len = 0;
            return;
        }

        // If there is a message in the TX queue, transmit it
        dequeue(&tx_msg_queue, data);
        *len = 8;
    }
}




/* MOBs */

mob_t cmd_rx_mob = {
    .mob_num = PAY_CMD_MOB_NUM,
    .mob_type = RX_MOB,
    .dlc = 8,
    .id_tag = { PAY_PAY_CMD_MOB_ID },
    .id_mask = { CAN_RX_MASK_ID },
    .ctrl = default_rx_ctrl,

    .rx_cb = cmd_rx_callback
};

mob_t cmd_tx_mob = {
    .mob_num = OBC_CMD_MOB_NUM,
    .mob_type = TX_MOB,
    .id_tag = { PAY_OBC_CMD_MOB_ID },
    .ctrl = default_tx_ctrl,

    .tx_data_cb = data_tx_callback
};
