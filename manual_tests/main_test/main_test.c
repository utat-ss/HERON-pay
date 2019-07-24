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
bool disable_hb = false;
// Set to true to print TX and RX CAN messages
bool print_can_msgs = false;


// Callback function signature to run a command
typedef void(*uart_cmd_fn_t)(void);

// UART-activated command
typedef struct {
    char* description;
    uart_cmd_fn_t fn;
} uart_cmd_t;


void get_status_fn(void);
void req_pay_hk_fn(void);
void req_pay_opt_fn(void);
void set_heaters_1_4_0c_fn(void);
void set_heaters_1_4_100c_fn(void);
void set_heater_5_0c_fn(void);
void set_heater_5_100c_fn(void);
void req_act_up_fn(void);
void req_act_down_fn(void);

// All possible commands
uart_cmd_t all_cmds[] = {
    {
        .description = "Get subsystem status",
        .fn = get_status_fn
    },
    {
        .description = "Request PAY HK data",
        .fn = req_pay_hk_fn
    },
    {
        .description = "Request PAY OPT data",
        .fn = req_pay_opt_fn
    },
    {
        .description = "Set heaters 1-4 setpoint = 0C",
        .fn = set_heaters_1_4_0c_fn
    },
    {
        .description = "Set heaters 1-4 setpoint = 100C",
        .fn = set_heaters_1_4_100c_fn
    },
    {
        .description = "Set heater 5 setpoint = 0C",
        .fn = set_heater_5_0c_fn
    },
    {
        .description = "Set heater 5 setpoint = 100C",
        .fn = set_heater_5_100c_fn
    },
    {
        .description = "Actuate plate up",
        .fn = req_act_up_fn
    },
    {
        .description = "Actuate plate down",
        .fn = req_act_down_fn
    }
};
// Length of array
const uint8_t all_cmds_len = sizeof(all_cmds) / sizeof(all_cmds[0]);





// Enqueues a message for PAY to receive
void enqueue_rx_msg(uint8_t msg_type, uint8_t field_number, uint32_t raw_data) {
    uint8_t rx_msg[8] = { 0x00 };
    rx_msg[0] = 0x00;
    rx_msg[1] = 0x00;
    rx_msg[2] = msg_type;
    rx_msg[3] = field_number;
    rx_msg[4] = (raw_data >> 24) & 0xFF;
    rx_msg[5] = (raw_data >> 16) & 0xFF;
    rx_msg[6] = (raw_data >> 8) & 0xFF;
    rx_msg[7] = raw_data & 0xFF;
    enqueue(&rx_msg_queue, rx_msg);
}


void process_pay_hk_tx(uint8_t field_num, uint32_t tx_data) {
    if (field_num == CAN_PAY_HK_TEMP) {
        // Printing floating point doesn't seem to work when it's in
        // the same print statement as the hex number
        // In testing, seemed to always print either "-2.000", "0.000", or "2.000" for the %f
        // Print it in a separate statement for now
        // TODO - investigate this

        double temperature = temp_raw_data_to_temperature(tx_data);
        print("Temperature: ");
        print("0x%.4X = ", tx_data);
        print("%.1f C\n", temperature);
    }

    else if (field_num == CAN_PAY_HK_HUM) {
        double humidity = hum_raw_data_to_humidity(tx_data);
        print("Humidity: ");
        print("0x%.4X = ", tx_data);
        print("%.1f %%RH\n", humidity);
    }

    else if (field_num == CAN_PAY_HK_PRES) {
        double pressure = pres_raw_data_to_pressure(tx_data);
        print("Pressure: ");
        print("0x%.6lX = ", tx_data);
        print("%.1f kPa\n", pressure);
    }

    else if ((CAN_PAY_HK_THERM0 <= field_num) &&
            (field_num < CAN_PAY_HK_THERM0 + 10)) {
        uint8_t channel = field_num - CAN_PAY_HK_THERM0;
        double vol = adc_raw_data_to_raw_vol(tx_data);
        double res = therm_vol_to_res(vol);
        double temp = therm_res_to_temp(res);
        print("Thermistor %u: 0x%.3X", channel, tx_data);
        print(" = %.3f C\n", temp);
    }

    else if (field_num == CAN_PAY_HK_HEAT_SP1) {
        double vol = dac_raw_data_to_vol(tx_data);
        double res = therm_vol_to_res(vol);
        double temp = therm_res_to_temp(res);
        print("Heater Setpoint 1: 0x%.3X", tx_data);
        print(" = %.3f C\n", temp);
    }

    else if (field_num == CAN_PAY_HK_HEAT_SP2) {
        double vol = dac_raw_data_to_vol(tx_data);
        double res = therm_vol_to_res(vol);
        double temp = therm_res_to_temp(res);
        print("Heater Setpoint 2: 0x%.3X", tx_data);
        print(" = %.3f C\n", temp);
    }

    else if (field_num == CAN_PAY_HK_PROX_LEFT) {
        double vol = adc_raw_data_to_raw_vol(tx_data);
        print("Left proximity: 0x%.3X = %.3f V\n", tx_data, vol);
    }

    else if (field_num == CAN_PAY_HK_PROX_RIGHT) {
        double vol = adc_raw_data_to_raw_vol(tx_data);
        print("Right proximity: 0x%.3X = %.3f V\n", tx_data, vol);
    }

    else {
        return;
    }

    uint8_t next_field_num = field_num + 1;
    if (next_field_num < CAN_PAY_HK_FIELD_COUNT) {
        enqueue_rx_msg(CAN_PAY_HK, next_field_num, 0);
    }
}

void process_pay_opt_tx(uint8_t field_num, uint32_t tx_data) {
    double percent = ((double) tx_data) / 0xFFFFFF * 100.0;
    print("Well #%u: ", field_num);
    print("0x%.6lX = ", tx_data);
    print("%.1f %%\n", percent);

    uint8_t next_field_num = field_num + 1;
    if (next_field_num < CAN_PAY_OPT_FIELD_COUNT) {
        enqueue_rx_msg(CAN_PAY_OPT, next_field_num, 0);
    }
}

void process_pay_ctrl_tx(uint8_t field_num) {
    if (field_num == CAN_PAY_CTRL_HEAT_SP1) {
        print("Set heaters 1-4 setpoint\n");
    } else if (field_num == CAN_PAY_CTRL_HEAT_SP2) {
        print("Set heater 5 setpoint\n");
    } else if (field_num == CAN_PAY_CTRL_ACT_UP) {
        print("Actuated plate up\n");
    } else if (field_num == CAN_PAY_CTRL_ACT_DOWN) {
        print("Actuated plate down\n");
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

    uint8_t msg_type = tx_msg[2];
    uint8_t field_num = tx_msg[3];
    uint32_t tx_data =
        ((uint32_t) tx_msg[4] << 24) |
        ((uint32_t) tx_msg[5] << 16) |
        ((uint32_t) tx_msg[6] << 8) |
        ((uint32_t) tx_msg[7]);

    switch (msg_type) {
        case CAN_PAY_HK:
            process_pay_hk_tx(field_num, tx_data);
            break;
        case CAN_PAY_OPT:
            process_pay_opt_tx(field_num, tx_data);
            break;
        case CAN_PAY_CTRL:
            process_pay_ctrl_tx(field_num);
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




void get_status_fn(void) {
    print("Restart count: %lu\n", restart_count);
    print("Restart reason: %lu\n", restart_reason);
    print("Uptime: %lu s\n", uptime_s);
}

void req_pay_hk_fn(void) {
    enqueue_rx_msg(CAN_PAY_HK, 0, 0);
}

void req_pay_opt_fn(void) {
    enqueue_rx_msg(CAN_PAY_OPT, 0, 0);
}

void set_heaters_1_4_0c_fn(void) {
    // TODO - encapsulate in function in lib-common/conversions
    double res = therm_temp_to_res(0);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    enqueue_rx_msg(CAN_PAY_CTRL, CAN_PAY_CTRL_HEAT_SP1, raw_data);
}
void set_heaters_1_4_100c_fn(void) {
    double res = therm_temp_to_res(100);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    enqueue_rx_msg(CAN_PAY_CTRL, CAN_PAY_CTRL_HEAT_SP1, raw_data);
}

void set_heater_5_0c_fn(void) {
    double res = therm_temp_to_res(0);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    enqueue_rx_msg(CAN_PAY_CTRL, CAN_PAY_CTRL_HEAT_SP2, raw_data);
}

void set_heater_5_100c_fn(void) {
    double res = therm_temp_to_res(100);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    enqueue_rx_msg(CAN_PAY_CTRL, CAN_PAY_CTRL_HEAT_SP2, raw_data);
}

void req_act_up_fn(void) {
    enqueue_rx_msg(CAN_PAY_CTRL, CAN_PAY_CTRL_ACT_UP, 0);
}

void req_act_down_fn(void) {
    enqueue_rx_msg(CAN_PAY_CTRL, CAN_PAY_CTRL_ACT_DOWN, 0);
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
    // Make sure the watchdog timer starts at the first thing, before any
    // initialization which could get stuck
    // Turn it off first to prevent repeated resets if the watchdog timer just
    // caused an MCU reset
    WDT_OFF();
    WDT_ENABLE_SYS_RESET(WDTO_8S);

    init_pay();

    print("\n\n\nStarting commands test\n\n");

    // Change these as necessary for testing
    sim_local_actions = false;
    sim_obc = true;
    hb_ping_period_s = 30;
    disable_hb = false;
    print_can_msgs = true;

    print("sim_local_actions = %u\n", sim_local_actions);
    print("sim_obc = %u\n", sim_obc);
    print("hb_ping_period_s = %lu\n", hb_ping_period_s);
    print("disable_hb = %u\n", disable_hb);
    print("print_can_msgs = %u\n", print_can_msgs);

    // Initialize heartbeat separately so we have the option to disable it for debugging
    if (!disable_hb) {
        init_hb(HB_PAY);
    }

    print("At any time, press h to show the command menu\n");
    print_cmds();
    set_uart_rx_cb(uart_cb);

    while(1) {
        WDT_ENABLE_SYS_RESET(WDTO_8S);

        if (!disable_hb) {
            run_hb();
        }

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            print_next_tx_msg();
            if (sim_obc) {
                sim_send_next_tx_msg();
            } else {
                send_next_tx_msg();
            }
        }

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            print_next_rx_msg();
            process_next_rx_msg();
        }
    }

    return 0;
}
