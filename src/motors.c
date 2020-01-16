/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

Motor specs: https://www.haydonkerkpittman.com/products/linear-actuators/can-stack-stepper/37mm-37000

- phase/enable mode

TODO - test faults - INT2 - Can manually force signal LOW and see what happens
*/

#include "motors.h"

// true if there is a fault detected in one or both of the motors
bool motor_fault = false;

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

/*
 * TODO this function is no longer valid as interrupt vector not connected
 * might do IO poll if necessary
ISR(INT2_vect) {

    print("INT2 - Motor Fault (PEX INTA)\n");

    // Check if either of the motor FLTn (fault) pins is low
    if (get_pex_pin(&pex1, PEX_B, MOT1_FLT_N) == 0) {
        motor_fault = true;
        print("MOTOR 1 IN FAULT\n");
    }
    if (get_pex_pin(&pex1, PEX_A, MOT2_FLT_N)  == 0) {
        motor_fault = true;
        print("MOTOR 2 IN FAULT\n");
    }

    if (motor_fault) {
        disable_motors();
    }
}
 */
