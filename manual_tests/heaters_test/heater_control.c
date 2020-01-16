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

/*
 * Utility function: return number of 1s in a binary string
 */

uint16_t count_ones(uint16_t num){
    uint16_t one_count = 0;
    //TO-DO: need a timer for this while loop
    while(num != 0){
        if ((num & 0x01) == 1){
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
        THERM_READINGS[i] = 0.07; //some specific double, this number doesn't matter anyway
        THERM_ERR_CODE[i] = 0;
    }
}

// TODO: not sure if this need to be atomic when polling ADC data, will see
void acquire_therm_data(void){
    // poll all ADC2 channels
    fetch_all_adc_channels(&adc2);

    //convert to temperature and store in THERM_READINGS
    for (uint8_t i = 0; i < 12; i++){
        uint16_t raw_data = read_adc_channel(&adc2, i);
        double therm_temp = adc_raw_to_therm_temp(raw_data);
        THERM_READINGS[i] = therm_temp;
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

void heater_toggle(double calc_num, uint8_t heater_num){
    //NOTE: heater_num here is the physical heater number - 1

    // hot case
    if(calc_num > SETPOINT){
        if(HEATERS_STATUS & (0x01 << heater_num)){
            //heater ON, need to turn it OFF
            heater_off(heater_num+1);
            HEATERS_STATUS = HEATERS_STATUS & ~(0x01 << heater_num);
        }
    }
    // cold case
    else if(calc_num < SETPOINT){
        if(!(HEATERS_STATUS & (0x01 << heater_num))){
            //heater OFF, need to turn it ON
            heater_on(heater_num+1);
            HEATERS_STATUS = HEATERS_STATUS | (0x01 << heater_num);
        }
    }
}

// fine I decided to hardcode the heaters based on physical setup
void heater_3in_ctrl(void){
    //looking at heater1 & 3, remember to minus one for bit shift in function argument
    double avg_reading = 0.0;
    double sum = 0.0;
    uint8_t normal_therm_num = 0;

    // heater 2
    // averaging TH3-5
    for(uint8_t i = 3; i < 6; i++){
        if(THERM_ERR_CODE[i] == 0){
            sum += THERM_READINGS[i];
            normal_therm_num += 1;
        }
    }

    avg_reading = (sum/normal_therm_num);
    heater_toggle(avg_reading, 1);

    // heater 4
    // averaging TH9-11
    sum = 0.0;
    normal_therm_num = 0;
    for(uint8_t i = 9; i < 12; i++){
        if(THERM_ERR_CODE[i] == 0){
            sum += THERM_READINGS[i];
            normal_therm_num += 1;
        }
    }

    avg_reading = (sum/normal_therm_num);
    heater_toggle(avg_reading, 3);

}

void heater_4in_ctrl(void){
    //looking at heater2 & 4, remember to minus one for bit shift in function argument
    //
    // if want to introduce weighted average should do it here
    // could incorporate the weight into error code
    double avg_reading = 0.0;
    double sum = 0.0;
    uint8_t normal_therm_num = 0;

    // heater 1
    // averaging TH0-2 and TH11
    for(uint8_t i = 0; i < 3; i++){
        if(THERM_ERR_CODE[i] == 0){
            sum += THERM_READINGS[i];
            normal_therm_num += 1;
        }
    }

    if(THERM_ERR_CODE[11] == 0){
        sum += THERM_READINGS[11];
        normal_therm_num += 1;
    }

    avg_reading = (sum/normal_therm_num);
    heater_toggle(avg_reading, 0);

    // heater 3
    // averaging TH5-8
    sum = 0.0;
    normal_therm_num = 0;
    for(uint8_t i = 5; i < 9; i++){
        if(THERM_ERR_CODE[i] == 0){
            sum += THERM_READINGS[i];
            normal_therm_num += 1;
        }
    }

    avg_reading = (sum/normal_therm_num);
    heater_toggle(avg_reading, 2);

}

void heater_5in_ctrl(void){
    // looking at heater 5 here, average everything
    double avg_reading = 0.0;
    double sum = 0.0;
    uint8_t normal_therm_num = 0;

    for(uint8_t i = 0; i < 12; i++){
        if(THERM_ERR_CODE[i] == 0){
            sum += THERM_READINGS[i];
            normal_therm_num += 1;
        }
    }

    avg_reading = (sum/normal_therm_num);
    heater_toggle(avg_reading, 4);
}

void average_heaters(void){
    heater_3in_ctrl();
    heater_4in_ctrl();
    heater_5in_ctrl();
}

void heater_ctrl_status(void){
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

void heater_control(void){
    print("TH1,TH2,TH3,TH4,TH5,TH6,TH7,TH8,TH9,TH10,TH11,TH12,H1,H2,H3,H4,H5\n");
    while(1){
        acquire_therm_data();
        eliminate_bad_therm();
        average_heaters();

        // a debugging function full of print statements
        heater_ctrl_status();

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
