/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

Motor specs: https://www.haydonkerkpittman.com/products/linear-actuators/can-stack-stepper/37mm-37000

- phase/enable mode

TODO - sleep motors - can achieve by disable boost 10V
TODO - test faults - INT2
TODO - change function calls based on pins re-mapping
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
    // M1, M0, decay tied to both
    /*
    // nSLEEP = 1
    set_pex_pin_dir(&pex1, PEX_A, MOT_SLP, OUTPUT);
    set_pex_pin_dir(&pex1, PEX_B, MOT_SLP, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT_SLP, 1);
    set_pex_pin(&pex1, PEX_B, MOT_SLP, 1);

    // ADECAY = 1 (fast decay)
    // TODO - might need to connect BDECAY high in hardware (currently tied to ground)
    set_pex_pin_dir(&pex1, PEX_A, MOT_ADECAY, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT_ADECAY, 1);

    // M1 = 1 (async fast decay)
    set_pex_pin_dir(&pex1, PEX_A, MOT_M1, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT_M1, 1);

    // APHASE = 0
    set_pex_pin_dir(&pex1, PEX_A, MOT_APHASE, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT_APHASE, 0);

    // BPHASE = 0
    set_pex_pin_dir(&pex1, PEX_A, MOT_BPHASE, OUTPUT);
    set_pex_pin_dir(&pex1, PEX_B, MOT_BPHASE, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT_BPHASE, 0);
    set_pex_pin(&pex1, PEX_B, MOT_BPHASE, 0);

    // AENBL = 0
    set_pex_pin_dir(&pex1, PEX_A, MOT_AENBL, OUTPUT);
    set_pex_pin_dir(&pex1, PEX_B, MOT_AENBL, OUTPUT);
    set_pex_pin(&pex1, PEX_A, MOT_AENBL, 0);
    set_pex_pin(&pex1, PEX_B, MOT_AENBL, 0);

    // BENBL = 0
    init_cs(MOT_BENBL_PIN, &MOT_BENBL_DDR);
    set_cs_low(MOT_BENBL_PIN, &MOT_BENBL_PORT);
     */
}

void enable_motors(void) {
    // Enable motors and disable sleep

    // nSLEEP = 1

    // AENBL = 1
    /*
    set_pex_pin(&pex1, PEX_A, MOT_AENBL, 1);
    set_pex_pin(&pex1, PEX_B, MOT_AENBL, 1);

    // BENBL = 1
    set_cs_high(MOT_BENBL_PIN, &MOT_BENBL_PORT);
     */
}

void disable_motors(void) {
    // Disable motors and enable sleep

    // nSLEEP = 0

    // AENBL = 0
    /*
    set_pex_pin(&pex1, PEX_A, MOT_AENBL, 0);
    set_pex_pin(&pex1, PEX_B, MOT_AENBL, 0);

    // BENBL = 0
    set_cs_low(MOT_BENBL_PIN, &MOT_BENBL_PORT);
     */
}


/*
period - time for one cycle (in ms)
       - this is in the ideal case - only considering delays, assuming pin switching is instantaneous
num_cycles - number of cycles (of `period` ms) to actuate for
forward - true to go "forward", false to go "backward"
        - these are arbitrary and just mean opposite directions
*/
void actuate_motors(uint16_t period, uint16_t num_cycles, bool forward) {
    /*
    enable_motors();

    uint16_t delay = period / 4;

    for (uint16_t i = 0; i < num_cycles; i++) {
        if (forward) {
            // BPHASE = 1
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_BPHASE, 1);
            set_pex_pin(&pex1, PEX_B, MOT_BPHASE, 1);

            // APHASE = 1
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_APHASE, 1);

            // BPHASE = 0
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_BPHASE, 0);
            set_pex_pin(&pex1, PEX_B, MOT_BPHASE, 0);

            // APHASE = 0
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_APHASE, 0);
        }

        else {
            // APHASE = 1
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_APHASE, 1);

            // BPHASE = 1
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_BPHASE, 1);
            set_pex_pin(&pex1, PEX_B, MOT_BPHASE, 1);

            // APHASE = 0
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_APHASE, 0);

            // BPHASE = 0
            delay_ms(delay);
            set_pex_pin(&pex1, PEX_A, MOT_BPHASE, 0);
            set_pex_pin(&pex1, PEX_B, MOT_BPHASE, 0);
        }
    }

    disable_motors();
     */
}




ISR(INT2_vect) {
    /*
    print("INT2 - Motor Fault (PEX INTA)\n");

    // Check if either of the motor FLTn (fault) pins is low
    if (get_pex_pin(&pex1, PEX_A, MOT_FLT_A) == 0) {
        motor_fault = true;
        print("MOTOR A\n");
    }
    if (get_pex_pin(&pex1, PEX_A, MOT_FLT_B)  == 0) {
        motor_fault = true;
        print("MOTOR B\n");
    }

    if (motor_fault) {
        disable_motors();
    }
     */
}
