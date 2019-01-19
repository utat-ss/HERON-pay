/*
A test for all PAY functionality by simulating sending and receiving CAN messages.
RX and TX are defined from PAY's perspective.
*/

#include <can/can.h>
#include <can/data_protocol.h>
#include <can/ids.h>
#include <uart/uart.h>

#include "../../src/general.h"

// Set to false to stop printing PAY's TX and RX CAN messages
bool print_can_msgs = true;


// Callback function signature to run a command
typedef void(*uart_cmd_fn_t)(void);

// UART-activated command
typedef struct {
    char* description;
    uart_cmd_fn_t fn;
} uart_cmd_t;


void req_pay_hk_fn(void);
void req_pay_sci_fn(void);
void req_pay_motor_fn(void);

// All possible commands
uart_cmd_t all_cmds[] = {
    {
        .description = "Request PAY HK data",
        .fn = req_pay_hk_fn
    },
    {
        .description = "Request PAY SCI data",
        .fn = req_pay_sci_fn
    },
    {
        .description = "Request PAY Motor actuation",
        .fn = req_pay_motor_fn
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
    // Must declare this here because redeclaring it in multiple case statements
    // gives an error
    uint32_t raw_data;

    switch (tx_msg[2]) {
        case CAN_PAY_HK_TEMP:
            // Printing floating point doesn't seem to work when it's in
            // the same print statement as the hex number
            // In testing, seemed to always print either "-2.000", "0.000", or "2.000" for the %f
            // Print it in a separate statement for now
            // TODO - investigate this

            print("Temperature: ");
            raw_data =
                (((uint32_t) tx_msg[4]) << 8) |
                ((uint32_t) tx_msg[5]);
            double temperature = temp_raw_data_to_temperature(raw_data);
            print("0x%.4X = ", raw_data);
            print("%.3f C\n", temperature);
            break;

        case CAN_PAY_HK_HUM:
            print("Humidity: ");
            raw_data =
                (((uint32_t) tx_msg[4]) << 8) |
                ((uint32_t) tx_msg[5]);
            double humidity = hum_raw_data_to_humidity(raw_data);
            print("0x%.4X = ", raw_data);
            print("%.3f %%RH\n", humidity);
            break;

        case CAN_PAY_HK_PRES:
            print("Pressure: ");
            raw_data =
                (((uint32_t) tx_msg[3]) << 16) |
                (((uint32_t) tx_msg[4]) << 8) |
                ((uint32_t) tx_msg[5]);
            double pressure = pres_raw_data_to_pressure(raw_data);
            print("0x%.6X = ", raw_data);
            print("%.3f kPa\n", pressure);
            break;

        default:
            break;
    }

    if (tx_msg[2] < CAN_PAY_HK_GET_COUNT - 1) {
        enqueue_rx_msg(CAN_PAY_HK, tx_msg[2] + 1);
    }
}

void process_pay_sci_tx(uint8_t* tx_msg) {
    print("Optical #%u: ", tx_msg[2]);
    uint32_t raw_data =
        (((uint32_t) tx_msg[3]) << 16) |
        (((uint32_t) tx_msg[4]) << 8) |
        ((uint32_t) tx_msg[5]);
    double percent = ((double) raw_data) / 0xFFFFFF * 100.0;
    print("0x%.6X = ", raw_data);
    print("%.3f %%\n", percent);

    if (tx_msg[2] < CAN_PAY_SCI_GET_COUNT - 1) {
        enqueue_rx_msg(CAN_PAY_OPT, tx_msg[2] + 1);
    }
}

void process_pay_motor_tx(uint8_t* tx_msg) {
    print("Motor actuated successfully\n");
}

// Processes a response that PAY sends back
void process_tx(void) {
    if (queue_empty(&tx_msg_queue)) {
        return;
    }

    uint8_t tx_msg[8] = { 0x00 };
    dequeue(&tx_msg_queue, tx_msg);

    switch (tx_msg[1]) {
        case CAN_PAY_HK:
            process_pay_hk_tx(tx_msg);
            break;
        case CAN_PAY_OPT:
            process_pay_sci_tx(tx_msg);
            break;
        case CAN_PAY_EXP:
            process_pay_motor_tx(tx_msg);
            break;
        default:
            break;
    }
}

void print_next_tx_msg(void) {
    if (queue_empty(&tx_msg_queue)) {
        return;
    }
    print("PAY TX: ");
    print_bytes(tx_msg_queue.content[tx_msg_queue.head], QUEUE_DATA_SIZE);
}

void print_next_rx_msg(void) {
    if (queue_empty(&rx_msg_queue)) {
        return;
    }
    print("PAY RX: ");
    print_bytes(rx_msg_queue.content[rx_msg_queue.head], QUEUE_DATA_SIZE);
}




void req_pay_hk_fn(void) {
    enqueue_rx_msg(CAN_PAY_HK, 0);
}

void req_pay_sci_fn(void) {
    enqueue_rx_msg(CAN_PAY_OPT, 0);
}

void req_pay_motor_fn(void) {
    enqueue_rx_msg(CAN_PAY_EXP, 0);
}




void print_cmds(void) {
    for (uint8_t i = 0; i < all_cmds_len; i++) {
        print("%u: %s\n", i, all_cmds[i].description);
    }
}

uint8_t uart_cb(const uint8_t* data, uint8_t len) {
    if (len == 0) {
        print("No UART\n");
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

    print("\n\n\n\nInitialized\n\n");

    print("At any time, press h to show the command menu\n");
    print_cmds();
    set_uart_rx_cb(uart_cb);

    while(1) {
        if (print_can_msgs) {
            print_next_tx_msg();
        }
        process_tx();

        if (print_can_msgs) {
            print_next_rx_msg();
        }
        process_next_rx_msg();
    }

    return 0;
}
