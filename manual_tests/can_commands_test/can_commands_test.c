#include <can/can.h>
#include <can/data_protocol.h>
#include <can/ids.h>
#include <uart/uart.h>

#include "../../src/env_sensors.h"



void tx_callback(uint8_t* data, uint8_t* len) {
    print("\nTX Callback\n");

    *len = 8;
    for (uint8_t i = 0; i < 8; ++i) {
        data[i] = 0;
    }

    switch (req_num) {
        case 0:
            data[0] = CAN_PAY_SCI;
            data[1] = PAY_SCI_TEMD;
            break;

        // case 3:
        case 1:
            data[0] = PAY_SCI_REQ;
            data[1] = PAY_SCI_SFH;
            break;

        default:
            req_num = 0;
            break;
    }

    req_num = (req_num + 1) % NUM_REQ_NUMS;

    print("Transmitting Message:\n");
    print_bytes(data, *len);
}


void rx_callback(const uint8_t* data, uint8_t len) {
    print("RX Callback\n");
    print("Received Message:\n");
    print_bytes((uint8_t *) data, len);

    if (data[0] == PAY_HK_REQ) {
        uint16_t raw_temperature = 0;
        uint32_t raw_humidity = 0;
        double converted;

        switch (data[1]) {
            case PAY_TEMP_1:
                raw_temperature = ((uint16_t) data[3] << 8) | (uint16_t) data[4];
                converted = convert_temperature(raw_temperature);
                print("--------\n");
                print("Temperature: %d C\n",(int)converted);
                print("--------\n");

                break;

            case PAY_HUMID_1:
                raw_humidity = ((uint32_t) data[3] << 24) | ((uint32_t) data[4] << 16);
                converted = convert_humidity(raw_humidity);
                print("--------\n");
                print("Humidity: %d percent\n",(int)converted);
                print("--------\n");

                break;

            default:
                break;
        }
    }

    if (data[0] == PAY_SCI_REQ) {
        switch (data[1]) {
            case PAY_SCI_TEMD:
                print("TEMD\n");
                break;

            case PAY_SCI_SFH:
                print("SFH\n");
                break;

            default:
                break;
        }

        uint32_t raw_optical =
                ((uint32_t) data[2] << 16) |
                ((uint32_t) data[3] << 8 ) |
                ((uint32_t) data[4]      );
        uint32_t raw_optical_max = 0xFFFFFF;
		print("%lu / %lu\n", raw_optical, raw_optical_max);

        double fraction = ((double) raw_optical / (double) raw_optical_max);
        print("--------\n");
        print("Optical: %d percent\n", (uint8_t) (fraction * 100.0));
        print("--------\n");
    }
}




int main(void) {
    init_uart();
    print("\n\nUART Initialized\n");

    init_can();
    init_tx_mob(&tx_mob);
    init_rx_mob(&rx_mob);
    print("CAN Initialized\n");

    while(1) {
        resume_mob(&tx_mob);
        while (!is_paused(&tx_mob)) {};
        _delay_ms(2000);
    }
}
