#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/boost.h"
#include "../../src/heaters.h"

int main(void){
    init_uart();
    print("\nUART initialized\n");

    init_adc(&adc2);
    print("\nADC2 initialized\n");

    init_spi();
    print("\nSPI initialized\n");

    init_pex(&pex2);
    print("\nPEX2 initialized\n");

    init_uptime();

    init_heater_ctrl();
    print("\nHeaters Initialized\n");

    init_boosts ();
    enable_6V_boost ();
    print("\n6V boost turned on\n");

    print("\nStarting Heaters control test\n\n");




    // Set update period
    heater_ctrl_period_s = 5;

    // Setpoints
    // set_heaters_setpoint_raw(0x2DD);    // 10 C
    // set_heaters_setpoint_raw(0x328);    // 14 C
    // set_heaters_setpoint_raw(0x400);    // 25 C
    // set_heaters_setpoint_raw(0x411);    // 26 C
    // set_heaters_setpoint_raw(0x423);    // 27 C
    // set_heaters_setpoint_raw(0x436);    // 28 C
    set_heaters_setpoint_raw(0x44A);    // 29 C
    // set_heaters_setpoint_raw(0x45E);    // 30 C
    // set_heaters_setpoint_raw(0x492);    // 33 C

    // Get raw values for testing setpoints
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(10.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(14.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(25.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(26.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(27.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(28.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(29.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(30.0));
    // print("0x%x\n", heater_setpoint_to_dac_raw_data(33.0));

    // Uncomment to reset all statuses
    // for (uint8_t i = 0; i < THERMISTOR_COUNT; i++) {
    //     therm_err_codes = THERM_ERR_CODE_NORMAL;
    // }




    // Run once immediately
    run_heater_ctrl();
    while (1) {
        heater_ctrl_main();
    }
}
