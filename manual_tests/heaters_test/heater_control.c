#include <adc/adc.h>
#include <conversions/conversions.h>
#include <pex/pex.h>
#include <uart/uart.h>

#include "../../src/heaters.h"

//define global variables

//temperature constants, all in degree Celsius
#define SETPOINT 37
#define ULL -40
#define UHL 120

uint16_t THERM_STATUS;
uint8_t HEATERS_STATUS;
double THERM_READINGS[12];
uint8_t THERM_ERR_CODE[12];

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

/*
 * Utility function: return number of 1s in a binary string
 */

uint16_t count_ones(uint16_t num){
    uint16_t one_count = 0;
    //TO-DO: need a timer for this while loop
    while(num != 0){
        if (num & 0x01 == 1){
            one_count += 1;
        }
        num = num >> 1;
    }
    return one_count;
}


void init_control_loop(void){
    // 0 means elminated, 1 means normal
    THERM_STATUS = 0x0fff;

    // 0 means OFF, 1 means ON
    HEATERS_STATUS = 0x00;
    for (uint8_t i = 0; i < 12; i++){
        THERM_READINGS[i] = 0.07 //some specific double, this number doesn't matter anyway
        THERM_ERR_CODE[i] = 0;
    }
}

// TO-DO: not sure if this need to be atomic when polling ADC data, will see
void acquire_therm_data(void){
    // poll all ADC2 channels
    fetch_all_adc_channels(&adc2);

    //convert to temperature and store in THERM_READINGS
    for (uint8_t i = 0; i < 12; i++){
        double temp = adc_raw_to_therm_temp(adc2->channel_data[i]);
        THERM_READINGS[i] = temp;
    }
}

void eliminate_bad_therm(){
    for(uint8_t i = 0; i < 12; i++){
        //compare with both lower and upper limits first
        if(THERM_READINGS[i] < ULL){
            THERM_STATUS = THERM_STATUS & (~(0x1 << i));
            THERM_ERR_CODE[i] = 1;
        }
        else if(THERM_READINGS[i] > UHL){
            THERM_STATUS = THERM_STATUS & (~(0x1 << i));
            THERM_ERR_CODE[i] = 2;
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

void heater_control(void){
    acquire_therm_data();
    eliminate_bad_therm();
    average_heaters();
    control_output();
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

=======
=======
    print("\nStarting Heaters control test\n\n");

>>>>>>> skeleton of control loop
    set_uart_rx_cb(key_pressed);
    while (1) {}
>>>>>>> heaters manual test done
}
