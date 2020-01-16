#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/boost.h"
#include "../../src/heaters.h"

//define global variables

//temperature constants, all in degree Celsius
#define SETPOINT 33
#define ULL -40
#define UHL 120

uint16_t THERM_STATUS;
uint8_t HEATERS_STATUS;
double THERM_READINGS[12];
uint8_t THERM_ERR_CODE[12];

// TODO: write a case where all thermistors are eliminated (rip) so we don't divide by zero
// TODO: include math.h for sqrt function (and log for temperature conversion)

/*
 * ABOUT THERM_ERR_CODE
 *
 * This variable is an indication of why the thermistor failed, or say at which point of the control loop it is eliminated
 * 0 - normal/not eliminated
 * 1 - lower than ultra low limit (ULL)
 * 2 - higher than ultra high limit (UHL)
 * The following are some future consideration
 * 3 - lower than mean (miu) of all 12 thermistors by 3 standard deviation (sigma)
 * 4 - higher than miu by 3 sigma
 * 5, 6, 7 - should not happen
 *
 */

void eliminate_bad_therm_sim(){
    for(uint8_t i = 0; i < 12; i++){
        //compare with both lower and upper limits first
        if(THERM_READINGS[i] < ULL){
            THERM_STATUS = THERM_STATUS & (~(0x1 << i));
            THERM_ERR_CODE[i] = 1;
        }
        else if(THERM_READINGS[i] > UHL){
            THERM_STATUS = THERM_STATUS & (~(0x1 << i));
            THERM_ERR_CODE[i] = 2;
        }else{
            //otherwise assume normal, recovery
            THERM_ERR_CODE[i] = 0;
        }
    }

    //compute mean
    //
    /*
    double sum = 0.0;
    for(uint8_t i = 0; i < 12; i++){
        if (THERM_ERR_CODE[i] == 0){
            sum += THERM_READINGS[i];
        }
    }
    uint16_t valid_therm_num = count_ones(THERM_STATUS);
    double miu = sum/valid_therm_num;*/

    //compute standard deviation - no square root function, stash for now
    //
    /*sum = 0.0;
    for(uint8_t i = 0; i < 12; i++){
        if (THERM_ERR_CODE[i] == 0){
            double diff = THERM_READINGS[i] - miu;
            sum += diff * diff;
        }
    }
    double variance = sum/(valid_therm_num - 1);*/
}


void heater_ctrl_status_print(void){
    //print thermistors status
    for(uint8_t i = 0; i < 12; i++){
        if(THERM_ERR_CODE[i] == 0){
            //print("Thermistor #: %d, Reading: %.5f\n", i, THERM_READINGS[i]);
            print("%.5f,",THERM_READINGS[i]);
        }
        else{
            //print("Thermistor #: %d, Reading: %.5f, Err Status: %d\n", i, THERM_READINGS[i], THERM_ERR_CODE[i]);
            print("Err%d,", THERM_ERR_CODE[i]);
        }
    }

    //print heater status
    //print("heater status %d\n", HEATERS_STATUS);
    for(uint8_t i = 0; i < 5; i++){
        uint8_t heater_check = HEATERS_STATUS;
        if(heater_check & (0x01 << i)){
            //print("Heater #: %d, Status: ON\n", i+1);
            print("%d,", 1);
        }
        else{
            //print("Heater #: %d, Status: OFF\n", i+1);
            print("%d,", 0);
        }
    }
    print("\n");
}

void heater_control_sim(void){
    print("TH1,TH2,TH3,TH4,TH5,TH6,TH7,TH8,TH9,TH10,TH11,TH12,H1,H2,H3,H4,H5\n");
    while(1){
        acquire_therm_data();
        eliminate_bad_therm_sim();
        average_heaters();

        // a debugging function full of print statements
        heater_ctrl_status_print();

        // current delay, every 20 seconds
        _delay_ms(20000);
    }
}

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
    if (len == 0) {
        return 0;
    }

    switch (buf[0]) {
        case 'e':
            print("Control loop starts\n");
            init_control_loop ();
            heater_control();
            break;
        default:
            print("Invalid command\n");
            break;
    }

    return 1;
}

int main(void){
    init_uart();
    print("\nUART initialized\n");

    init_adc(&adc2);
    print("\nADC2 initialized\n");

    init_spi();
    print("\nSPI initialized\n");

    init_pex(&pex2);
    print("\nPEX2 initialized\n");

    init_heaters();
    print("\nHeaters Initialized\n");

    init_boosts ();
    enable_6V_boost ();
    print("\n6V boost turned on\n");

    print("\nStarting Heaters control test\n\n");

    set_uart_rx_cb(key_pressed);
    while (1) {}
}
