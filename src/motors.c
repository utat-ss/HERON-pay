/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

Motor specs: https://www.haydonkerkpittman.com/products/linear-actuators/can-stack-stepper/37mm-37000

- phase/enable mode

TODO - sleep motors
TODO - test faults
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

    // nSLEEP = 1
    pex_set_pin_dir(&pex, MOT_SLP, PEX_A, OUTPUT);
    pex_set_pin_dir(&pex, MOT_SLP, PEX_B, OUTPUT);
    pex_set_pin(&pex, MOT_SLP, PEX_A, HIGH);
    pex_set_pin(&pex, MOT_SLP, PEX_B, HIGH);

    // ADECAY = 1 (fast decay)
    // TODO - might need to connect BDECAY high in hardware (currently tied to ground)
    pex_set_pin_dir(&pex, MOT_ADECAY, PEX_A, OUTPUT);
    pex_set_pin(&pex, MOT_ADECAY, PEX_A, HIGH);

    // M1 = 1 (async fast decay)
    pex_set_pin_dir(&pex, MOT_M1, PEX_A, OUTPUT);
    pex_set_pin(&pex, MOT_M1, PEX_A, HIGH);

    // APHASE = 0
    pex_set_pin_dir(&pex, MOT_APHASE, PEX_A, OUTPUT);
    pex_set_pin(&pex, MOT_APHASE, PEX_A, LOW);

    // BPHASE = 0
    pex_set_pin_dir(&pex, MOT_BPHASE, PEX_A, OUTPUT);
    pex_set_pin_dir(&pex, MOT_BPHASE, PEX_B, OUTPUT);
    pex_set_pin(&pex, MOT_BPHASE, PEX_A, LOW);
    pex_set_pin(&pex, MOT_BPHASE, PEX_B, LOW);

    // AENBL = 0
    pex_set_pin_dir(&pex, MOT_AENBL, PEX_A, OUTPUT);
    pex_set_pin_dir(&pex, MOT_AENBL, PEX_B, OUTPUT);
    pex_set_pin(&pex, MOT_AENBL, PEX_A, LOW);
    pex_set_pin(&pex, MOT_AENBL, PEX_B, LOW);

    // BENBL = 0
    init_cs(MOT_BENBL_PIN, &MOT_BENBL_DDR);
    set_cs_low(MOT_BENBL_PIN, &MOT_BENBL_PORT);
}

void enable_motors(void) {
    // Enable motors and disable sleep

    // nSLEEP = 1

    // AENBL = 1
    pex_set_pin(&pex, MOT_AENBL, PEX_A, HIGH);
    pex_set_pin(&pex, MOT_AENBL, PEX_B, HIGH);

    // BENBL = 1
    set_cs_high(MOT_BENBL_PIN, &MOT_BENBL_PORT);
}

void disable_motors(void) {
    // Disable motors and enable sleep

    // nSLEEP = 0

    // AENBL = 0
    pex_set_pin(&pex, MOT_AENBL, PEX_A, LOW);
    pex_set_pin(&pex, MOT_AENBL, PEX_B, LOW);

    // BENBL = 0
    set_cs_low(MOT_BENBL_PIN, &MOT_BENBL_PORT);
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
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, HIGH);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, HIGH);

            // APHASE = 1
            delay_ms(delay);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, HIGH);

            // BPHASE = 0
            delay_ms(delay);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, LOW);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, LOW);

            // APHASE = 0
            delay_ms(delay);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, LOW);
        }

        else {
            // APHASE = 1
            delay_ms(delay);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, HIGH);

            // BPHASE = 1
            delay_ms(delay);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, HIGH);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, HIGH);

            // APHASE = 0
            delay_ms(delay);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, LOW);

            // BPHASE = 0
            delay_ms(delay);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, LOW);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, LOW);
        }
    }

    disable_motors();
}




// TODO - should this be INT2 or PCINT2?
ISR(PCINT2_vect) {
    print("Interrupt - PCINT2 - Motor Fault (PEX INTA)\n");

    // GPA0 = _FLT_A_
    // GPA1 = _FLT_B_

    // Check if either of the motor _FLT_ (fault) pins is low
    // TODO - make pin constants
    // TODO - should these be combined into a single PEX read?

    if (pex_get_pin(&pex, 0, PEX_A) == 0) {
        motor_fault = true;
        print("MOTOR A FAULT\n");
    }
    if (pex_get_pin(&pex, 1, PEX_A)  == 0) {
        motor_fault = true;
        print("MOTOR B FAULT\n");
    }

    if (motor_fault) {
        disable_motors();
    }
}

ISR(INT2_vect) {
    print("Interrupt - INT2\n");
}
