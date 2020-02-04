/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

Motor specs: https://www.haydonkerkpittman.com/products/linear-actuators/can-stack-stepper/37mm-37000

- phase/enable mode
*/

#include "motors.h"

#define PERIOD_MS   100
#define NUM_CYCLES  1

// true if there is a fault detected in one or both of the motors
// TODO IO poll for motor fault probably in HK data
bool motor_fault = false;
uint16_t last_exec_time_motors = 0;
uint8_t motor_routine_status;


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

// TODO step count, add one more status for count unequal - tilted plate
// TODO CAN comm, communicate motor routine status
void motors_routine(void){

    //enable 10V boost converter
    enable_10V_boost();
    disable_6V_boost();

    // when limit switch not pressed, pex pin reading should return 0
    uint8_t switch1_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT1_PRESSED);
    uint8_t switch2_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT2_PRESSED);
    while(!switch1_pressed &&
          !switch2_pressed &&
          (uptime_s - last_exec_time_motors < 30)){
        // actuate one motor downwards at a time
        actuate_motor1 (PERIOD_MS, NUM_CYCLES, true);
        actuate_motor2 (PERIOD_MS, NUM_CYCLES, true);

        //update switch status
        switch1_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT1_PRESSED);
        switch2_pressed = get_pex_pin(&pex2, PEX_A, LIM_SWT2_PRESSED);
    }

    //check if timed out
    if (uptime_s - last_exec_time_motors > 30){
        motor_routine_status = MOTOR_ROUTINE_TIMEOUT;
        last_exec_time_motors = uptime_s;
        return;
    } else {
        motor_routine_status = MOTOR_ROUTINE_DONE;
        last_exec_time_motors = uptime_s;

        disable_10V_boost();
        enable_6V_boost();
        return;
    }

    // should not reach here
    return;
}
