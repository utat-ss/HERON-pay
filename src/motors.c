/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

Motor specs: https://www.haydonkerkpittman.com/products/linear-actuators/can-stack-stepper/37mm-37000

- phase/enable mode only
*/

#include "motors.h"

#define PERIOD_MS   100
#define NUM_CYCLES  1
#define MAX_STEP 300
#define MAX_COUNT 10

// true if there is a fault detected in one or both of the motors
// TODO IO poll for motor fault probably in HK data?
bool motor_fault = false;
uint32_t last_exec_time_motors = 0;
uint8_t motor_routine_status = 0;


// Define this delay function because the built-in _delay_ms() only works with
// compile-time constants
void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        _delay_ms(1);
    }
}

void init_motors(void) {
    // nSLEEP = 0, logic LOW for device to sleep
    set_pex_pin_dir(&pex1, PEX_B, MOT1_SLP_N, OUTPUT);
    set_pex_pin_dir(&pex1, PEX_B, MOT2_SLP_N, OUTPUT);
    set_pex_pin(&pex1, PEX_B, MOT1_SLP_N, 0);
    set_pex_pin(&pex1, PEX_B, MOT2_SLP_N, 0);

    // ADECAY = 1 & BDECAY = 1
    // TODO - double check datasheet about setting DECAY, might want slow DECAY
    set_pex_pin_dir(&pex1, PEX_B, MOT1_ADECAY, OUTPUT);
    set_pex_pin(&pex1, PEX_B, MOT1_ADECAY, 1);
    set_pex_pin_dir(&pex1, PEX_A, MOT2_ADECAY, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT2_ADECAY, 1);

    set_pex_pin_dir(&pex1, PEX_B, MOT1_BDECAY, OUTPUT);
    set_pex_pin(&pex1, PEX_B, MOT1_ADECAY, 1);
    set_pex_pin_dir(&pex1, PEX_A, MOT2_BDECAY, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT2_BDECAY, 1);

    // M1 = 1 (async fast decay)
    set_pex_pin_dir(&pex1, PEX_B, MOT1_M1, OUTPUT);
    set_pex_pin(&pex1, PEX_B, MOT1_M1, 1);
    set_pex_pin_dir(&pex1, PEX_A, MOT2_M1, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT2_M1, 1);

    // AENBL = 0
    set_pex_pin_dir(&pex1, PEX_B, MOT1_AENBL, OUTPUT);
    set_pex_pin_dir(&pex1, PEX_A, MOT2_AENBL, OUTPUT);
    set_pex_pin(&pex1, PEX_B, MOT1_AENBL, 0);
    set_pex_pin(&pex1, PEX_A, MOT2_AENBL, 0);

    // BENBL = 0
    set_pex_pin_dir(&pex1, PEX_B, MOT1_BENBL, OUTPUT);
    set_pex_pin_dir(&pex1, PEX_A, MOT2_BENBL, OUTPUT);
    set_pex_pin(&pex1, PEX_B, MOT1_BENBL, 0);
    set_pex_pin(&pex1, PEX_A, MOT2_BENBL, 0);

    // APHASE = 0 & BPHASE = 0
    init_cs(MOT1_APHASE_PIN, &MOT1_APHASE_DDR);
    set_cs_low(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);
    init_cs(MOT1_BPHASE_PIN, &MOT1_BPHASE_DDR);
    set_cs_low(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);

    init_cs(MOT2_APHASE_PIN, &MOT2_APHASE_DDR);
    set_cs_low(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);
    init_cs(MOT2_BPHASE_PIN, &MOT2_BPHASE_DDR);
    set_cs_low(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);
}

void enable_motors(void) {
    // Enable motors and disable sleep

    // nSLEEP = 1, logic HIGH to enable device
    set_pex_pin(&pex1, PEX_B, MOT1_SLP_N, 1);
    set_pex_pin(&pex1, PEX_B, MOT2_SLP_N, 1);

    // AENBL = 1
    set_pex_pin(&pex1, PEX_B, MOT1_AENBL, 1);
    set_pex_pin(&pex1, PEX_A, MOT2_AENBL, 1);

    // BENBL = 1
    set_pex_pin(&pex1, PEX_B, MOT1_BENBL, 1);
    set_pex_pin(&pex1, PEX_A, MOT2_BENBL, 1);
}

void enable_motor1(void) {
    // Enable motor1 only

    // nSLEEP = 1
    set_pex_pin(&pex1, PEX_B, MOT1_SLP_N, 1);

    // AENBL = 1
    set_pex_pin(&pex1, PEX_B, MOT1_AENBL, 1);

    // BENBL = 1
    set_pex_pin(&pex1, PEX_B, MOT1_BENBL, 1);
}

void enable_motor2(void) {
    // Enable motor2 only

    // nSLEEP = 1
    set_pex_pin(&pex1, PEX_B, MOT2_SLP_N, 1);

    // AENBL = 1
    set_pex_pin(&pex1, PEX_A, MOT2_AENBL, 1);

    // BENBL = 1
    set_pex_pin(&pex1, PEX_A, MOT2_BENBL, 1);
}

void disable_motors(void) {
    // Disable motors and enable sleep

    // nSLEEP = 0
    set_pex_pin(&pex1, PEX_B, MOT1_SLP_N, 0);
    set_pex_pin(&pex1, PEX_B, MOT2_SLP_N, 0);

    // AENBL = 0
    set_pex_pin(&pex1, PEX_B, MOT1_AENBL, 0);
    set_pex_pin(&pex1, PEX_A, MOT2_AENBL, 0);

    // BENBL = 0
    set_pex_pin(&pex1, PEX_B, MOT1_BENBL, 0);
    set_pex_pin(&pex1, PEX_A, MOT2_BENBL, 0);
}

void disable_motor1(void) {
    // Disable motor1 only

    // nSLEEP = 0
    set_pex_pin(&pex1, PEX_B, MOT1_SLP_N, 0);

    // AENBL = 0
    set_pex_pin(&pex1, PEX_B, MOT1_AENBL, 0);

    // BENBL = 0
    set_pex_pin(&pex1, PEX_B, MOT1_BENBL, 0);
}

void disable_motor2(void) {
    // Disable motor2 only

    // nSLEEP = 0
    set_pex_pin(&pex1, PEX_B, MOT2_SLP_N, 0);

    // AENBL = 0
    set_pex_pin(&pex1, PEX_A, MOT2_AENBL, 0);

    // BENBL = 0
    set_pex_pin(&pex1, PEX_A, MOT2_BENBL, 0);
}

/*
period - time for one cycle (in ms)
       - this is in the ideal case - only considering delays, assuming pin switching is instantaneous
num_cycles - number of cycles (of `period` ms) to actuate for
forward - true to go "forward", false to go "backward"
        - these are arbitrary and just mean opposite directions
*/
void actuate_motors(uint16_t period, uint16_t num_cycles, bool forward) {

    enable_motors();

    uint16_t delay = period / 4;

    for (uint16_t i = 0; i < num_cycles; i++) {
        if (forward) {
            // BPHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);
            set_cs_high(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);

            // APHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);
            set_cs_high(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);

            // BPHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);
            set_cs_low(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);

            // APHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);
            set_cs_low(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);
        }

        else {
            // APHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);
            set_cs_low(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);

            // BPHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);
            set_cs_low(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);

            // APHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);
            set_cs_high(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);

            // BPHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);
            set_cs_high(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);
        }
    }

    disable_motors();
}

void actuate_motor1(uint16_t period, uint16_t num_cycles, bool forward) {

    enable_motor1();

    uint16_t delay = period / 4;

    for (uint16_t i = 0; i < num_cycles; i++) {
        if (forward) {
            // BPHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);

            // APHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);

            // BPHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);

            // APHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);
        }

        else {
            // APHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);

            // BPHASE = 0
            delay_ms(delay);
            set_cs_low(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);

            // APHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_APHASE_PIN, &MOT1_APHASE_PORT);

            // BPHASE = 1
            delay_ms(delay);
            set_cs_high(MOT1_BPHASE_PIN, &MOT1_BPHASE_PORT);
        }
    }

    disable_motor1();
}

void actuate_motor2(uint16_t period, uint16_t num_cycles, bool forward) {

    enable_motor2();

    uint16_t delay = period / 4;

    for (uint16_t i = 0; i < num_cycles; i++) {
        if (forward) {
           // BPHASE = 1
            delay_ms(delay);
            set_cs_high(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);

            // APHASE = 1
            delay_ms(delay);
            set_cs_high(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);

            // BPHASE = 0
            delay_ms(delay);
            set_cs_low(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);

            // APHASE = 0
            delay_ms(delay);
            set_cs_low(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);
        }

        else {
            // APHASE = 0
            delay_ms(delay);
            set_cs_low(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);

            // BPHASE = 0
            delay_ms(delay);
            set_cs_low(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);

            // APHASE = 1
            delay_ms(delay);
            set_cs_high(MOT2_APHASE_PIN, &MOT2_APHASE_PORT);

            // BPHASE = 1
            delay_ms(delay);
            set_cs_high(MOT2_BPHASE_PIN, &MOT2_BPHASE_PORT);
        }
    }

    disable_motor2();
}

// TODO tilted plate can be recovered from limit switch reading
void motors_routine(void){

    WDT_ENABLE_SYS_RESET(WDTO_8S);

    // enable 10V boost converter
    enable_10V_boost();
    // delay for power to settle - 5s
    delay_ms(5000);

    WDT_ENABLE_SYS_RESET(WDTO_8S);

    disable_6V_boost();

    last_exec_time_motors = uptime_s;

    // when limit switch not pressed, pex pin reading should return 0
    uint8_t switch1_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT1_PRESSED);
    uint8_t switch2_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT2_PRESSED);

    // number of times each motor actuated
    uint16_t count_mot1 = 0;
    uint16_t count_mot2 = 0;

    // limit switch debounce counter to prevent random fluctuation
    // set it to maximum 5 count
    uint8_t count_lim_switch1 = 0;
    uint8_t count_lim_switch2 = 0;

    // move up the motors for 15 seconds
    while(count_mot1 < 150 && count_mot2 < 150){
        actuate_motor1 (PERIOD_MS, NUM_CYCLES, false);
        count_mot1 += 1;
        actuate_motor2 (PERIOD_MS, NUM_CYCLES, false);
        count_mot2 += 1;
        WDT_ENABLE_SYS_RESET(WDTO_8S);
    }

    count_mot1 = 0;
    count_mot2 = 0;

    while((count_lim_switch1 < MAX_COUNT && count_lim_switch2 < MAX_COUNT) &&
          (count_mot1 < MAX_STEP && count_mot2 < MAX_STEP)){
        // actuate one motor downwards at a time
        actuate_motor1 (PERIOD_MS, NUM_CYCLES, true);
        count_mot1 += 1;
        actuate_motor2 (PERIOD_MS, NUM_CYCLES, true);
        count_mot2 += 1;

        //update switch status
        switch1_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT1_PRESSED);
        if(switch1_pressed){
            count_lim_switch1 += 1;
        }

        switch2_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT2_PRESSED);
        if(switch2_pressed){
            count_lim_switch2 += 1;
        }

        //tilt recovery check
        while(count_lim_switch1 != count_lim_switch2){

            //move motors
            if(count_lim_Switch1 > count_lim_switch2){
                actuate_motor2 (PERIOD_MS, NUM_CYCLES, true);
                count_mot2 += 1;
            }
            if(count_lim_Switch1 < count_lim_switch2){
                actuate_motor1 (PERIOD_MS, NUM_CYCLES, true);
                count_mot1 += 1;
            }

            //update switch status
            switch1_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT1_PRESSED);
            if(switch1_pressed){
                count_lim_switch1 += 1;
            }

            switch2_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT2_PRESSED);
            if(switch2_pressed){
                count_lim_switch2 += 1;
            }
        }

        WDT_ENABLE_SYS_RESET(WDTO_8S);
    }

    //check if timed out
    if ((count_mot1 == MAX_STEP) || (count_mot2 == MAX_STEP)){
        motor_routine_status = MOTOR_ROUTINE_TIMEOUT;

    } else {
        motor_routine_status = MOTOR_ROUTINE_DONE;
        // only update when routine executed successfully
        last_exec_time_motors = uptime_s;
    }

    WDT_ENABLE_SYS_RESET(WDTO_8S);

    // should not reach here
    disable_10V_boost();
    enable_6V_boost();
    return;
}
