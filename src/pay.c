/*
General-use functions for PAY.
*/

#include "pay.h"

pin_info_t adc_cs = {
    .port = &ADC_CS_PORT_PAY,
    .ddr = &ADC_CS_DDR_PAY,
    .pin = ADC_CS_PIN_PAY
};

adc_t adc = {
    .channels = 0x0fff, // channels 0-11
    .cs = &adc_cs
};


pin_info_t pex_cs = {
    .port = &PEX_CS_PORT_PAY,
    .ddr = &PEX_CS_DDR_PAY,
    .pin = PEX_CS_PIN_PAY
};

pin_info_t pex_rst = {
    .port = &PEX_RST_PORT_PAY,
    .ddr = &PEX_RST_DDR_PAY,
    .pin = PEX_RST_PIN_PAY
};

pex_t pex = {
    .addr = PEX_ADDR_PAY,
    .cs = &pex_cs,
    .rst = &pex_rst
};


// Initializes everything in PAY
void init_pay(void) {
    // UART
    init_uart();
    print("\n\nInitializing PAY\n");
    print("UART\n");

    // SPI
    init_spi();
    print("SPI\n");

    // PEX
    init_pex(&pex);
    print("PEX\n");

    // ADC
    init_adc(&adc);
    print("ADC\n");

    // Environmental sensors
    temp_init();
    hum_init();
    pres_init();
    print("Sensors\n");

    // PAY-Optical
    opt_spi_init();
    print("Optical\n");

    // Motors
    init_motors();
    print("Motors\n");

    // Queues
    init_queue(&can_rx_msgs);
    init_queue(&can_tx_msgs);
    print("Queues\n");

    // CAN and MOBs
    init_can();
    init_rx_mob(&cmd_rx_mob);
    init_tx_mob(&data_tx_mob);
    print("CAN\n");
}

// If there is an RX messsage in the queue, handle it
void handle_next_rx_msg(void) {
    if (!queue_empty(&can_rx_msgs)) {
        handle_rx_msg();
    }
}

/*
If there is a TX message in the queue, send it

When resume_mob(mob name) is called, it:
1) resumes the MOB
2) triggers an interrupt (callback function) to get the data to transmit
3) sends the data
4) pauses the mob
*/
void send_next_tx_msg(void) {
    if (!queue_empty(&can_tx_msgs)) {
        resume_mob(&data_tx_mob);
    }
}
