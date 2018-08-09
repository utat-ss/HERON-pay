/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

- indexer mode
- slow decay (0% of cycle in fast decay)
- 16 microsteps/step
*/

#include "motors.h"

// true if there is a fault detected in one or both of the motors
bool motor_fault = false;


void init_motors(void) {
    // M1, M0, decay tied to both

    // nSLEEP = 1
    pex_set_pin_dir_a(PEX_ADDR_PAY, MOT_SLP, PEX_DIR_OUTPUT);
    pex_set_pin_dir_b(PEX_ADDR_PAY, MOT_SLP, PEX_DIR_OUTPUT);
    pex_set_pin_high_a(PEX_ADDR_PAY, MOT_SLP);
    pex_set_pin_high_b(PEX_ADDR_PAY, MOT_SLP);

    // ADECAY = 1 (fast decay)
    // TODO - might need to connect BDECAY high in hardware
    // (currently tied to ground)
    pex_set_pin_dir_a(PEX_ADDR_PAY, MOT_ADECAY, PEX_DIR_OUTPUT);
    pex_set_pin_high_a(PEX_ADDR_PAY, MOT_ADECAY);

    // M1 = 1 (async fast decay)
    pex_set_pin_dir_a(PEX_ADDR_PAY, MOT_M1, PEX_DIR_OUTPUT);
    pex_set_pin_high_a(PEX_ADDR_PAY, MOT_M1);

    // APHASE = 0
    pex_set_pin_dir_a(PEX_ADDR_PAY, MOT_APHASE, PEX_DIR_OUTPUT);
    pex_set_pin_low_a(PEX_ADDR_PAY, MOT_APHASE);

    // BPHASE = 0
    pex_set_pin_dir_a(PEX_ADDR_PAY, MOT_BPHASE, PEX_DIR_OUTPUT);
    pex_set_pin_dir_b(PEX_ADDR_PAY, MOT_BPHASE, PEX_DIR_OUTPUT);
    pex_set_pin_low_a(PEX_ADDR_PAY, MOT_BPHASE);
    pex_set_pin_low_b(PEX_ADDR_PAY, MOT_BPHASE);

    // AENBL = 0
    pex_set_pin_dir_a(PEX_ADDR_PAY, MOT_AENBL, PEX_DIR_OUTPUT);
    pex_set_pin_dir_b(PEX_ADDR_PAY, MOT_AENBL, PEX_DIR_OUTPUT);
    pex_set_pin_low_a(PEX_ADDR_PAY, MOT_AENBL);
    pex_set_pin_low_b(PEX_ADDR_PAY, MOT_AENBL);

    // BENBL = 0
    init_cs(MOT_BENBL_PIN, &MOT_BENBL_DDR);
    set_cs_low(MOT_BENBL_PIN, &MOT_BENBL_PORT);
}

// void enable_motors(void) {
//     // Enable motors and disable sleep
//     pex_set_pin_low_a(PEX_ADDR_PAY, PEX_EN);
//     pex_set_pin_low_b(PEX_ADDR_PAY, PEX_EN);
//     // pex_set_pin_high_a(PEX_ADDR_PAY, PEX_SLP);
//     // pex_set_pin_high_b(PEX_ADDR_PAY, PEX_SLP);
// }
//
// void disable_motors(void) {
//     // Disable motors and enable sleep
//     pex_set_pin_high_a(PEX_ADDR_PAY, PEX_EN);
//     pex_set_pin_high_b(PEX_ADDR_PAY, PEX_EN);
//     // pex_set_pin_low_a(PEX_ADDR_PAY, PEX_SLP);
//     // pex_set_pin_low_b(PEX_ADDR_PAY, PEX_SLP);
// }
//
// bool step_high = true;

void actuate_motors(void) {
    print("Actuating motors\n");

    // TODO - faults
    // if (motor_fault) {
    //     return;
    // }

    // enable_motors();

    // AENBL = 1
    pex_set_pin_high_a(PEX_ADDR_PAY, MOT_AENBL);
    pex_set_pin_high_b(PEX_ADDR_PAY, MOT_AENBL);

    // BENBL = 1
    set_cs_high(MOT_BENBL_PIN, &MOT_BENBL_PORT);

    while (1) {
        for (uint8_t i = 0; i < 20; i++) {
            // BPHASE = 1
            _delay_ms(5);
            pex_set_pin_high_a(PEX_ADDR_PAY, MOT_BPHASE);
            pex_set_pin_high_b(PEX_ADDR_PAY, MOT_BPHASE);

            // APHASE = 1
            _delay_ms(5);
            pex_set_pin_high_a(PEX_ADDR_PAY, MOT_APHASE);

            // BPHASE = 0
            _delay_ms(5);
            pex_set_pin_low_a(PEX_ADDR_PAY, MOT_BPHASE);
            pex_set_pin_low_b(PEX_ADDR_PAY, MOT_BPHASE);

            // APHASE = 0
            _delay_ms(5);
            pex_set_pin_low_a(PEX_ADDR_PAY, MOT_APHASE);

            print("Loop\n");
        }

        print("Flip\n");
        print("Waiting 1 second\n");
        _delay_ms(1000);

        for (uint8_t i = 0; i < 20; i++) {
            // APHASE = 1
            _delay_ms(5);
            pex_set_pin_high_a(PEX_ADDR_PAY, MOT_APHASE);

            // BPHASE = 1
            _delay_ms(5);
            pex_set_pin_high_a(PEX_ADDR_PAY, MOT_BPHASE);
            pex_set_pin_high_b(PEX_ADDR_PAY, MOT_BPHASE);

            // APHASE = 0
            _delay_ms(5);
            pex_set_pin_low_a(PEX_ADDR_PAY, MOT_APHASE);

            // BPHASE = 0
            _delay_ms(5);
            pex_set_pin_low_a(PEX_ADDR_PAY, MOT_BPHASE);
            pex_set_pin_low_b(PEX_ADDR_PAY, MOT_BPHASE);

            print("Loop\n");
        }

        print("Flip\n");
        print("Waiting 1 second\n");
        _delay_ms(1000);
    }

    // disable_motors();

    print("Done actuating motors\n");
}



// TODO - should this be INT2 or PCINT2?
ISR(PCINT2_vect) {
    print("Interrupt - Motor Fault - PCINT2 (Pin change interrupt 2, PEX INTA)\n");

    // GPA0 = _FLT_A_
    // GPA1 = _FLT_B_

    // Check if either of the motor _FLT_ (fault) pins is low
    uint8_t gpioa = pex_read(PEX_ADDR_PAY, PEX_GPIO_BASE);
    if ((gpioa & _BV(0)) == 0) {
        motor_fault = true;
        print("MOTOR A FAULT\n");
    }
    if ((gpioa & _BV(1)) == 0) {
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
