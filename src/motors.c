/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

- indexer mode
- slow decay (0% of cycle in fast decay)
- 16 microsteps/step
*/

#include "motors.h"

// true if there is a fault detected in one or both of the motors
bool motor_fault = false;


pin_info_t pex_cs = {
    .port = &PEX_CS_PORT_PAY,
    .ddr = &PEX_CS_DDR_PAY,
    .pin = PEX_CS_PIN_PAY
};

pin_info_t pex_rst = {
    .port = &PEX_RST_PORT_PAY,
    .ddr = &PEX_RST_DDR_PAY,
    .pin = PEX_RST_PIN_PAY
};

pex_t pex = {
    .addr = PEX_ADDR_PAY,
    .cs = &pex_cs,
    .rst = &pex_rst
};

void init_motors(void) {
    // M1, M0, decay tied to both

    // nSLEEP = 1
    pex_set_pin_dir(&pex, MOT_SLP, PEX_A, OUTPUT);
    pex_set_pin_dir(&pex, MOT_SLP, PEX_B, OUTPUT);
    pex_set_pin(&pex, MOT_SLP, PEX_A, HIGH);
    pex_set_pin(&pex, MOT_SLP, PEX_B, HIGH);

    // ADECAY = 1 (fast decay)
    // TODO - might need to connect BDECAY high in hardware
    // (currently tied to ground)
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
    // pex_set_pin_high_a(PEX_ADDR_PAY, PEX_SLP);
    // pex_set_pin_high_b(PEX_ADDR_PAY, PEX_SLP);

    // AENBL = 1
    pex_set_pin(&pex, MOT_AENBL, PEX_A, HIGH);
    pex_set_pin(&pex, MOT_AENBL, PEX_B, HIGH);

    // BENBL = 1
    set_cs_high(MOT_BENBL_PIN, &MOT_BENBL_PORT);
}

void disable_motors(void) {
    // Disable motors and enable sleep
    // pex_set_pin_low_a(PEX_ADDR_PAY, PEX_SLP);
    // pex_set_pin_low_b(PEX_ADDR_PAY, PEX_SLP);

    // AENBL = 0
    pex_set_pin(&pex, MOT_AENBL, PEX_A, LOW);
    pex_set_pin(&pex, MOT_AENBL, PEX_B, LOW);

    // BENBL = 0
    set_cs_low(MOT_BENBL_PIN, &MOT_BENBL_PORT);
}


// forward - true to go "forward", false to go "backward"
// these are arbitrary and just mean opposite directions
void actuate_motors(bool forward) {
    print("Actuating motors\n");

    enable_motors();

    for (uint8_t i = 0; i < 20; i++) {
        print("Loop\n");

        if (forward) {
            // BPHASE = 1
            _delay_ms(50);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, HIGH);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, HIGH);

            // APHASE = 1
            _delay_ms(50);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, HIGH);

            // BPHASE = 0
            _delay_ms(50);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, LOW);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, LOW);

            // APHASE = 0
            _delay_ms(50);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, LOW);
        }

        else {
            // APHASE = 1
            _delay_ms(50);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, HIGH);

            // BPHASE = 1
            _delay_ms(50);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, HIGH);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, HIGH);

            // APHASE = 0
            _delay_ms(50);
            pex_set_pin(&pex, MOT_APHASE, PEX_A, LOW);

            // BPHASE = 0
            _delay_ms(50);
            pex_set_pin(&pex, MOT_BPHASE, PEX_A, LOW);
            pex_set_pin(&pex, MOT_BPHASE, PEX_B, LOW);
        }
    }

    disable_motors();

    print("Done actuating\n");
}




// TODO - should this be INT2 or PCINT2?
// TODO - test faults
ISR(PCINT2_vect) {
    print("Interrupt - Motor Fault - PCINT2 (Pin change interrupt 2, PEX INTA)\n");

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
        // TODO
        // disable_motors();
    }
}

ISR(INT2_vect) {
    print("INT2\n");
}
