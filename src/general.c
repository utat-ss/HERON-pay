/*
General-use functions for PAY.
*/

#include "general.h"


// Initializes everything in PAY
void init_pay(void) {
    // UART
    init_uart();
    // SPI
    init_spi();

    // PEX
    init_pex(&pex1);
    init_pex(&pex2);

    // ADC
    init_adc(&adc1);
    init_adc(&adc2);

    // Environmental sensors
    init_hum();
    init_pres();

    init_boosts();
    enable_6V_boost();
    
    init_heater_ctrl();

    // Motors
    init_motors();

    // Queues
    init_queue(&rx_msg_queue);
    init_queue(&tx_msg_queue);

    // PAY-Optical
    init_opt_spi();
    rst_opt_spi();

    // CAN and MOBs
    init_can();
    init_rx_mob(&cmd_rx_mob);
    init_tx_mob(&cmd_tx_mob);

    init_uptime();
    init_com_timeout();
}
