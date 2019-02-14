/*
A test for all PAY functionality by simulating sending and receiving CAN messages.
RX and TX are defined from PAY's perspective.
*/

#include <can/can.h>
#include <can/data_protocol.h>
#include <can/ids.h>
#include <uart/uart.h>

#include "../../src/general.h"

// True to simulate OBC requests
bool sim_obc = false;

// Set to true to print TX and RX CAN messages
bool print_can_msgs = false;


// Callback function signature to run a command
typedef void(*uart_cmd_fn_t)(void);

// UART-activated command
typedef struct {
    char* description;
    uart_cmd_fn_t fn;
} uart_cmd_t;


void req_pay_hk_fn(void);
void req_pay_opt_fn(void);
void req_pay_exp_fn(void);
void req_exp_level_fn(void);
void req_exp_pop_fn(void);

// All possible commands
uart_cmd_t all_cmds[] = {
    {
        .description = "Request PAY HK data",
        .fn = req_pay_hk_fn
    },
    {
        .description = "Request PAY OPT data",
        .fn = req_pay_opt_fn
    },
    {
        .description = "Request PAY EXP data",
        .fn = req_pay_exp_fn
    },
    {
        .description = "Level actuation plate",
        .fn = req_exp_level_fn
    },
    {
        .description = "Pop blister packs",
        .fn = req_exp_pop_fn
    }
};
// Length of array
const uint8_t all_cmds_len = sizeof(all_cmds) / sizeof(all_cmds[0]);





// Enqueues a message for PAY to receive
void enqueue_rx_msg(uint8_t msg_type, uint8_t field_number) {
    uint8_t rx_msg[8] = { 0x00 };
    rx_msg[0] = 0;    // TODO
    rx_msg[1] = msg_type;
    rx_msg[2] = field_number;
    enqueue(&rx_msg_queue, rx_msg);
}


void process_pay_hk_tx(uint8_t* tx_msg) {
    uint8_t field_num = tx_msg[2];
    uint32_t raw_data =
        (((uint32_t) tx_msg[3]) << 16) |
        (((uint32_t) tx_msg[4]) << 8) |
        ((uint32_t) tx_msg[5]);

    if (field_num == CAN_PAY_HK_TEMP) {
        // Printing floating point doesn't seem to work when it's in
        // the same print statement as the hex number
        // In testing, seemed to always print either "-2.000", "0.000", or "2.000" for the %f
        // Print it in a separate statement for now
        // TODO - investigate this

        double temperature = temp_raw_data_to_temperature(raw_data);
        print("Temperature: ");
        print("0x%.4X = ", raw_data);
        print("%.1f C\n", temperature);

    }

    else if (field_num == CAN_PAY_HK_HUM) {
        double humidity = hum_raw_data_to_humidity(raw_data);
        print("Humidity: ");
        print("0x%.4X = ", raw_data);
        print("%.1f %%RH\n", humidity);
    }

    else if (field_num == CAN_PAY_HK_PRES) {
        double pressure = pres_raw_data_to_pressure(raw_data);
        print("Pressure: ");
        print("0x%.6lX = ", raw_data);
        print("%.1f kPa\n", pressure);
    }

    else if ((CAN_PAY_HK_THERM0 <= field_num) &&
            (field_num < CAN_PAY_HK_THERM0 + 10)) {
        uint8_t channel = field_num - CAN_PAY_HK_THERM0;
        double vol = adc_raw_data_to_raw_vol(raw_data);
        double res = therm_vol_to_res(vol);
        double temp = therm_res_to_temp(res);
        print("Thermistor %u: 0x%.3X", channel, raw_data);
        print(" = %.3f C\n", temp);
    }

    else if (field_num == CAN_PAY_HK_GET_DAC1) {
        double vol = adc_raw_data_to_raw_vol(raw_data);
        double res = therm_vol_to_res(vol);
        double temp = therm_res_to_temp(res);
        print("DAC Setpoint 1: 0x%.3X = %.3f C\n", raw_data, temp);
    }

    else if (field_num == CAN_PAY_HK_GET_DAC2) {
        double vol = adc_raw_data_to_raw_vol(raw_data);
        double res = therm_vol_to_res(vol);
        double temp = therm_res_to_temp(res);
        print("DAC Setpoint 2: 0x%.3X = %.3f C\n", raw_data, temp);
    }

    else {
        return;
    }

    uint8_t next_field_num = tx_msg[2] + 1;
    if (next_field_num < CAN_PAY_HK_GET_COUNT) {
        enqueue_rx_msg(CAN_PAY_HK, next_field_num);
    }
}

void process_pay_opt_tx(uint8_t* tx_msg) {
    print("Well #%u: ", tx_msg[2]);
    uint32_t raw_data =
        (((uint32_t) tx_msg[3]) << 16) |
        (((uint32_t) tx_msg[4]) << 8) |
        ((uint32_t) tx_msg[5]);
    double percent = ((double) raw_data) / 0xFFFFFF * 100.0;
    print("0x%.6lX = ", raw_data);
    print("%.1f %%\n", percent);

    uint8_t next_field_num = tx_msg[2] + 1;
    if (next_field_num < CAN_PAY_SCI_GET_COUNT) {
        enqueue_rx_msg(CAN_PAY_OPT, next_field_num);
    }
}

void process_pay_exp_tx(uint8_t* tx_msg) {
    uint8_t field_num = tx_msg[2];
    uint32_t raw_data =
        (((uint32_t) tx_msg[3]) << 16) |
        (((uint32_t) tx_msg[4]) << 8) |
        ((uint32_t) tx_msg[5]);

    if (field_num == CAN_PAY_EXP_PROX_LEFT) {
        print("Left proximity: 0x%.3X\n", raw_data);
    } else if (field_num == CAN_PAY_EXP_PROX_RIGHT) {
        print("Right proximity: 0x%.3X\n", raw_data);
    } else if (field_num == CAN_PAY_EXP_LEVEL) {
        print("Levelled actuation plate\n");
    } else if (field_num == CAN_PAY_EXP_POP) {
        print("Popped blister packs\n");
    }

    uint8_t next_field_num = tx_msg[2] + 1;
    if (next_field_num <= CAN_PAY_EXP_PROX_RIGHT) {
        enqueue_rx_msg(CAN_PAY_EXP, next_field_num);
    }
}

// Displays the response that PAY sends back
void sim_send_next_tx_msg(void) {
    uint8_t tx_msg[8] = { 0x00 };
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&tx_msg_queue)) {
            return;
        }
        dequeue(&tx_msg_queue, tx_msg);
    }

    switch (tx_msg[1]) {
        case CAN_PAY_HK:
            process_pay_hk_tx(tx_msg);
            break;
        case CAN_PAY_OPT:
            process_pay_opt_tx(tx_msg);
            break;
        case CAN_PAY_EXP:
            process_pay_exp_tx(tx_msg);
            break;
        default:
            return;
    }
}

void print_next_tx_msg(void) {
    if (!print_can_msgs) {
        return;
    }

    uint8_t tx_msg[8] = { 0x00 };
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&tx_msg_queue)) {
            return;
        }
        peek_queue(&tx_msg_queue, tx_msg);
    }

    print("CAN TX: ");
    print_bytes(tx_msg, 8);
}

void print_next_rx_msg(void) {
    if (!print_can_msgs) {
        return;
    }

    uint8_t rx_msg[8] = { 0x00 };
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (queue_empty(&rx_msg_queue)) {
            return;
        }
        peek_queue(&rx_msg_queue, rx_msg);
    }

    print("CAN RX: ");
    print_bytes(rx_msg, 8);
}




void req_pay_hk_fn(void) {
    enqueue_rx_msg(CAN_PAY_HK, 0);
}

void req_pay_opt_fn(void) {
    enqueue_rx_msg(CAN_PAY_OPT, 0);
}

void req_pay_exp_fn(void) {
    enqueue_rx_msg(CAN_PAY_EXP, 0);
}

void req_exp_level_fn(void) {
    enqueue_rx_msg(CAN_PAY_EXP, CAN_PAY_EXP_LEVEL);
}

void req_exp_pop_fn(void) {
    enqueue_rx_msg(CAN_PAY_EXP, CAN_PAY_EXP_POP);
}




void print_cmds(void) {
    for (uint8_t i = 0; i < all_cmds_len; i++) {
        print("%u: %s\n", i, all_cmds[i].description);
    }
}

uint8_t uart_cb(const uint8_t* data, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    // Print the typed character
    print("%c\n", data[0]);

    // Check for printing the help menu
    if (data[0] == 'h') {
        print_cmds();
    }

    // Check for a valid command number
    else if ('0' <= data[0] && data[0] < '0' + all_cmds_len) {
        // Enqueue the selected command
        uint8_t i = data[0] - '0';
        all_cmds[i].fn();
    }

    else {
        print("Invalid command\n");
    }

    // Processed 1 character
    return 1;
}

int main(void) {
    init_pay();

    print("\n\n\nStarting commands test\n\n");

    // Change these as necessary for testing
    sim_local_actions = false;
    sim_obc = false;
    print_can_msgs = true;

    print("sim_local_actions = %u\n", sim_local_actions);
    print("sim_obc = %u\n", sim_obc);
    print("print_can_msgs = %u\n", print_can_msgs);

    print("At any time, press h to show the command menu\n");
    print_cmds();
    set_uart_rx_cb(uart_cb);

    while(1) {
        print_next_tx_msg();
        if (sim_obc) {
            sim_send_next_tx_msg();
        } else {
            send_next_tx_msg();
        }

        print_next_rx_msg();
        process_next_rx_msg();
    }

    return 0;
}
