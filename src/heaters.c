 /*
For pay-ssm v4, all heaters are controlled with software via PEX pins

For Ganymede, all heater control pins are tied to PEX1, GPIOB0

For now no PWM control, just turn it all ON or all OFF all the time.

PWM regulation can be considered if want to reduce payload power consumption.

Author: Lorna Lan
 */

// TODO: write a case where all thermistors are eliminated (rip) so we don't divide by zero
// TODO: include math.h for sqrt function (and log for temperature conversion)

#include <conversions/conversions.h>

#include "heaters.h"

uint16_t therm_readings_raw[THERMISTOR_COUNT];
// in C
double therm_readings_conv[THERMISTOR_COUNT];
uint8_t therm_err_codes[THERMISTOR_COUNT];
// 0 means elminated, 1 means normal
uint8_t therm_enables[THERMISTOR_COUNT];

uint16_t heaters_setpoint_raw = HEATERS_SETPOINT_RAW_DEFAULT;
// 0 means OFF, 1 means ON
uint8_t heater_enables[HEATER_COUNT];

uint32_t heater_ctrl_last_exec_time = 0;


void init_heater_ctrl(void){
    set_pex_pin_dir(&pex2, PEX_B, HEATER1_EN_N, OUTPUT);
    set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 1);

    set_pex_pin_dir(&pex2, PEX_B, HEATER2_EN_N, OUTPUT);
    set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 1);

    set_pex_pin_dir(&pex2, PEX_B, HEATER3_EN_N, OUTPUT);
    set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 1);

    set_pex_pin_dir(&pex2, PEX_B, HEATER4_EN_N, OUTPUT);
    set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 1);

    set_pex_pin_dir(&pex2, PEX_B, HEATER5_EN_N, OUTPUT);
    set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 1);

    for (uint8_t i = 0; i < THERMISTOR_COUNT; i++) {
        therm_readings_raw[i] = 0;
        //some specific double, this number doesn't matter anyway
        therm_readings_conv[i] = 0.07;
        // TODO - only write to EEPROM when this is changed by CAN command
        therm_err_codes[i] = (uint8_t) read_eeprom_or_default(
            THERM_ERR_CODE_EEPROM_ADDR_BASE + (4 * i), THERM_ERR_CODE_NORMAL);
        therm_enables[i] = 1;
    }

    heaters_setpoint_raw = (uint16_t) read_eeprom_or_default(
        HEATERS_SETPOINT_EEPROM_ADDR, HEATERS_SETPOINT_RAW_DEFAULT);

    for (uint8_t i = 0; i < HEATER_COUNT; i++) {
        heater_enables[i] = 0;
    }
    
    // TODO - maybe run control once?
}

void heater_all_on(void) {
    set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 0);
    set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 0);
}

void heater_all_off(void) {
    set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 1);
    set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 1);
}

void heater_on(uint8_t heater_num){
    switch(heater_num){
    case 1:
        set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 0);
        break;
    case 2:
        set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 0);
        break;
    case 3:
        set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 0);
        break;
    case 4:
        set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 0);
        break;
    case 5:
        set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 0);
        break;
    default:
        return;
        break;
    }
}

void heater_off(uint8_t heater_num){
    switch(heater_num){
    case 1:
        set_pex_pin(&pex2, PEX_B, HEATER1_EN_N, 1);
        break;
    case 2:
        set_pex_pin(&pex2, PEX_B, HEATER2_EN_N, 1);
        break;
    case 3:
        set_pex_pin(&pex2, PEX_B, HEATER3_EN_N, 1);
        break;
    case 4:
        set_pex_pin(&pex2, PEX_B, HEATER4_EN_N, 1);
        break;
    case 5:
        set_pex_pin(&pex2, PEX_B, HEATER5_EN_N, 1);
        break;
    default:
        return;
        break;
    }
}


//manual test copy in manual_tests/heaters_test/heater_ctrl.c
/*
 * Utility function: return number of 1s in a binary string
 */
uint16_t count_ones(uint8_t* array, uint8_t size){
    uint16_t one_count = 0;
    for (uint8_t i = 0; i < size; i++) {
        if (array[i]) {
            one_count += 1;
        }
    }

    return one_count;
}


/*
 * Utility function: fast inverse square root
 */
float fast_inverse_square_root(double number){
    long i;
    float x2, y;
    const float threehalfs = 1.5F;

    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // MAGIC NUMBER 
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration

    return y;
}


// does not need to be atomic when polling ADC data
void acquire_therm_data(void){
    // poll all ADC2 channels
    fetch_all_adc_channels(&adc2);

    //convert to temperature and store in therm_readings_raw
    for (uint8_t i = 0; i < THERMISTOR_COUNT; i++){
        therm_readings_raw[i] = read_adc_channel(&adc2, i);
        therm_readings_conv[i] = adc_raw_to_therm_temp(therm_readings_raw[i]);
    }
}


void eliminate_bad_therm(void){
    for(uint8_t i = 0; i < THERMISTOR_COUNT; i++){
        //bypass ground-set thermistors
        if((therm_err_codes[i] != THERM_ERR_CODE_MANUAL_INVALID) &&
                (therm_err_codes[i] != THERM_ERR_CODE_MANUAL_VALID)){
            //compare with both lower and upper limits first
            if(therm_readings_conv[i] < THERM_CONV_ULL){
                therm_enables[i] = 0;
                therm_err_codes[i] = THERM_ERR_CODE_BELOW_ULL;
            }
            else if(therm_readings_conv[i] > THERM_CONV_UHL){
                therm_enables[i] = 0;
                therm_err_codes[i] = THERM_ERR_CODE_ABOVE_UHL;
            }else{
                //otherwise assume normal, recovery
                // TODO - set enable to 1?
                therm_err_codes[i] = THERM_ERR_CODE_NORMAL;
            }
        }
    }

    //compute mean
    //
    double sum = 0.0;
    for(uint8_t i = 0; i < THERMISTOR_COUNT; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            sum += therm_readings_conv[i];
        }
    }
    uint16_t valid_therm_num = count_ones(therm_enables, THERMISTOR_COUNT);
    double miu = 0.0;
    if(valid_therm_num > 0){
        miu = sum/valid_therm_num;
    } else {
        //TODO some sort of warning about no valid thermistors
        return;
    }

    //compute standard deviation
    sum = 0.0;
    double variance = 0.0;
    double sigma = 0.0;
    for(uint8_t i = 0; i < THERMISTOR_COUNT; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            double diff = therm_readings_conv[i] - miu;
            sum += diff * diff;
        }
    }
    if(valid_therm_num > 0){
        //population std doesn't need -1
        variance = sum/valid_therm_num;
        sigma = (double) (fast_inverse_square_root(variance));
    } else {
        //TODO some sort of warning about not enough valid thermistors
        return;
    }

    // elimination based on standard deviation
    // again, bypass ground-set thermistors
    for(uint8_t i = 0; i < THERMISTOR_COUNT; i++){
        if((therm_err_codes[i] != THERM_ERR_CODE_MANUAL_INVALID) &&
                (therm_err_codes[i] !=THERM_ERR_CODE_MANUAL_VALID)){
            //compare with both lower and upper limits first
            if(therm_readings_conv[i] < (miu-3*sigma)){
                therm_enables[i] = 0;
                therm_err_codes[i] = THERM_ERR_CODE_BELOW_MIU_3SIG;
            }
            else if(therm_readings_conv[i] > (miu+3*sigma)){
                therm_enables[i] = 0;
                therm_err_codes[i] = THERM_ERR_CODE_ABOVE_MIU_3SIG;
            }else{
                //otherwise assume normal, recovery
                // TODO - set enable to 1?
                therm_err_codes[i] = 0;
            }
        }
    }
}


void heater_toggle(double calc_num, uint8_t heater_num){
    //NOTE: heater_num here is the physical heater number - 1

    // hot case
    if(calc_num > heaters_setpoint_raw){
        if(heater_enables[heater_num]){
            //heater ON, need to turn it OFF
            heater_off(heater_num+1);
            heater_enables[heater_num] = 0;
        }
    }
    // cold case
    else if(calc_num < heaters_setpoint_raw){
        if(!heater_enables[heater_num]){
            //heater OFF, need to turn it ON
            heater_on(heater_num+1);
            heater_enables[heater_num] = 1;
        }
    }
}


// fine I decided to hardcode the heaters based on physical setup
void heater_3in_ctrl(void){
    //looking at heater 2 & 4, remember to minus one for bit shift in function argument
    double avg_reading = 0.0;
    double sum = 0.0;
    uint8_t normal_therm_num = 0;

    // heater 2
    // averaging TH3-5
    for(uint8_t i = 3; i < 6; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            sum += therm_readings_conv[i];
            normal_therm_num += 1;
        }
    }

    if(normal_therm_num > 0){
        avg_reading = (sum/normal_therm_num);
        heater_toggle(avg_reading, 1);  
    } else {
        // no working thermistors, rip
        // need to do sth here for the love of God
        return;
    }

    // heater 4
    // averaging TH9-11
    sum = 0.0;
    normal_therm_num = 0;
    for(uint8_t i = 9; i < 12; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            sum += therm_readings_conv[i];
            normal_therm_num += 1;
        }
    }

    if(normal_therm_num > 0){
        avg_reading = (sum/normal_therm_num);
        heater_toggle(avg_reading, 3);
    } else {
        return;
    }

}


void heater_4in_ctrl(void){
    //looking at heater 1 & 3, remember to minus one for bit shift in function argument
    //
    // if want to introduce weighted average should do it here
    // could incorporate the weight into error code
    double avg_reading = 0.0;
    double sum = 0.0;
    uint8_t normal_therm_num = 0;

    // heater 1
    // averaging TH0-2 and TH11
    for(uint8_t i = 0; i < 3; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            sum += therm_readings_conv[i];
            normal_therm_num += 1;
        }
    }

    if((therm_err_codes[11] == THERM_ERR_CODE_NORMAL) ||
            (therm_err_codes[11] == THERM_ERR_CODE_MANUAL_VALID)){
        sum += therm_readings_conv[11];
        normal_therm_num += 1;
    }

    if(normal_therm_num > 0){
        avg_reading = (sum/normal_therm_num);
        heater_toggle(avg_reading, 0);
    } else {
        return;
    }

    // heater 3
    // averaging TH5-8
    sum = 0.0;
    normal_therm_num = 0;
    for(uint8_t i = 5; i < 9; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            sum += therm_readings_conv[i];
            normal_therm_num += 1;
        }
    }

    if(normal_therm_num > 0){
        avg_reading = (sum/normal_therm_num);
        heater_toggle(avg_reading, 2);
    } else {
        return;
    }

}


void heater_5in_ctrl(void){
    // looking at heater 5 here, average everything
    double avg_reading = 0.0;
    double sum = 0.0;
    uint8_t normal_therm_num = 0;

    for(uint8_t i = 0; i < 12; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            sum += therm_readings_conv[i];
            normal_therm_num += 1;
        }
    }

    if(normal_therm_num > 0){
        avg_reading = (sum/normal_therm_num);
        heater_toggle(avg_reading, 4);
    } else {
        return;
    }
}


void average_heaters(void){
    heater_3in_ctrl();
    heater_4in_ctrl();
    heater_5in_ctrl();
}


//TODO: need to do something difference for status function - comm thru CAN
void print_heater_ctrl_status(void){
    //print thermistors status
    for(uint8_t i = 0; i < THERMISTOR_COUNT; i++){
        if((therm_err_codes[i] == THERM_ERR_CODE_NORMAL) ||
                (therm_err_codes[i] == THERM_ERR_CODE_MANUAL_VALID)){
            print("Thermistor #: %d, Reading: %.5f\n", i, therm_readings_conv[i]);
            print("%.5f,",therm_readings_conv[i]);
        }
        else{
            print("Thermistor #: %d, Reading: %.5f, Err Status: %d\n", i, therm_readings_conv[i], therm_err_codes[i]);
            print("Err%d,", therm_err_codes[i]);
        }
    }

    //print heater status
    for(uint8_t i = 0; i < HEATER_COUNT; i++){
        if(heater_enables[i]){
            print("Heater #: %d, Status: ON\n", i+1);
            print("%d,", 1);
        }
        else{
            print("Heater #: %d, Status: OFF\n", i+1);
            print("%d,", 0);
        }
    }
    print("\n");
}


// TODO: need to modify this, no print statement
void run_heater_control(void){
    //print("TH1,TH2,TH3,TH4,TH5,TH6,TH7,TH8,TH9,TH10,TH11,TH12,H1,H2,H3,H4,H5\n");

    acquire_therm_data();
    eliminate_bad_therm();
    average_heaters();

    // store status to the correct place
    print_heater_ctrl_status();
}


// heater control loop to be called in main
// TODO: can also permanently disable this function if don't care about temperature regulation anymore
void heater_ctrl_main(void){
    // currently update every 5 minutes
    if((uptime_s - heater_ctrl_last_exec_time) < 300){
        return;
    } else {
        run_heater_control ();
        heater_ctrl_last_exec_time = uptime_s;
    }
}
