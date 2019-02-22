/*
Test heaters by typing in commands over UART to manually turn on and off heaters.
Also display thermistor measurements.
*/

#include <uart/uart.h>

#include "../../src/general.h"
#include "../../src/heaters.h"


// Callback function signature to run a command
typedef void(*uart_cmd_fn_t)(void);

// UART-activated command
typedef struct {
    char* description;
    uart_cmd_fn_t fn;
} uart_cmd_t;

void read_thermistor_data_fn(void);
void turn_heaters_1_4_on_fn(void);
void turn_heaters_1_4_off_fn(void);
void turn_heater_5_on_fn(void);
void turn_heater_5_off_fn(void);


// All possible commands
uart_cmd_t all_cmds[] = {
    {
        .description = "Read thermistor data",
        .fn = read_thermistor_data_fn
    },
    {
        .description = "Turn heaters 1-4 on",
        .fn = turn_heaters_1_4_on_fn
    },
    {
        .description = "Turn heaters 1-4 off",
        .fn = turn_heaters_1_4_off_fn
    },
    {
        .description = "Turn heater 5 on",
        .fn = turn_heater_5_on_fn
    },
    {
        .description = "Turn heater 5 off",
        .fn = turn_heater_5_off_fn
    },

};
// Length of array
const uint8_t all_cmds_len = sizeof(all_cmds) / sizeof(all_cmds[0]);

// Modify this array to contain the ADC channels you want to monitor
// (channels 10 and 11 are something else - motor positioning sensors)
uint8_t adc_channels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
// uint8_t adc_channels[] = {0, 1, 2};
// automatically calculate the length of the array
uint8_t adc_channels_len = sizeof(adc_channels) / sizeof(adc_channels[0]);




void read_thermistor_data_fn(void) {
    //Read all of the thermistors' voltage from adc
    fetch_all_adc_channels(&adc);

    print("\n");
    print("Channel, Temperature (C), Raw (12 bits), Voltage (V), Resistance (kohms)\n");

    //Find resistance for each channel
    //only calculate it for the thermistors specified in adc_channels
    for (uint8_t i = 0; i < adc_channels_len; i++) {
        // Read ADC channel data
        uint8_t channel = adc_channels[i];
        uint16_t raw_data = read_adc_channel(&adc, channel);

        double voltage = adc_raw_data_to_raw_vol(raw_data);
        //Convert adc voltage to resistance of thermistor
        double resistance = therm_vol_to_res(voltage);
        //Convert resistance to temperature of thermistor
        double temperature = therm_res_to_temp(resistance);

        print("%u: %.3f, 0x%.3X, %.3f, %.3f\n", channel, temperature, raw_data, voltage, resistance);
    }
}

void turn_heaters_1_4_on_fn(void) {
    set_heaters_1_4_setpoint_temp(100);
    print("Set heaters 1-4 setpoint (DAC A) = 100 C\n");
    print("Heaters 1-4 should be ON\n");
}

void turn_heaters_1_4_off_fn(void) {
    set_heaters_1_4_setpoint_temp(0);
    print("Set heaters 1-4 setpoint (DAC A) = 0 C\n");
    print("Heaters 1-4 should be OFF\n");
}

void turn_heater_5_on_fn(void) {
    set_heater_5_setpoint_temp(100);
    print("Set heater 5 setpoint (DAC B) = 100 C\n");
    print("Heater 5 should be ON\n");
}

void turn_heater_5_off_fn(void) {
    set_heater_5_setpoint_temp(0);
    print("Set heater 5 setpoint (DAC B) = 0 C\n");
    print("Heater 5 should be OFF\n");
}




void print_cmds(void) {
    print("\n");
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
        print_cmds();
    }

    else {
        print("Invalid command\n");
    }

    // Processed 1 character
    return 1;
}

int main(void) {
    init_uart();
    init_spi();
    init_dac(&dac);
    init_adc(&adc);

    print("\n\n\nStarting test\n\n");

    turn_heaters_1_4_off_fn();
    turn_heater_5_off_fn();

    print("\nAt any time, press h to show the command menu\n");
    print_cmds();
    set_uart_rx_cb(uart_cb);

    while (1) {}

    return 0;
}
