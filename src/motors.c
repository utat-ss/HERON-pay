/*
Control for the DRV8834 (296-41246-1-ND on DigiKey) motor controller.

- indexer mode
- slow decay (0% of cycle in fast decay)
- 16 microsteps/step
*/

#include "motors.h"

// true if there is a fault detected in one or both of the motors
bool motor_fault = false;


// TODO - check how to read port expander pins for (FLT/FAULT)

void init_motors(void) {
    // Set M1 = 1, M1 = 1 (16 microsteps/step, p. 16)
    pex_set_dir_a(PEX_M1, PEX_DIR_OUTPUT);
    pex_set_gpio_a_high(PEX_M1);
    pex_set_dir_a(PEX_M0, PEX_DIR_OUTPUT);
    pex_set_gpio_a_high(PEX_M0);

    // Set _EN_ high (disabled)
    pex_set_dir_a(PEX_EN, PEX_DIR_OUTPUT);
    pex_set_gpio_a_high(PEX_EN);
    pex_set_dir_b(PEX_EN, PEX_DIR_OUTPUT);
    pex_set_gpio_b_high(PEX_EN);

    // Set _SLP_ (sleep) low (enter sleep state)
    pex_set_dir_a(PEX_SLP, PEX_DIR_OUTPUT);
    pex_set_gpio_a_low(PEX_SLP);
    pex_set_dir_b(PEX_SLP, PEX_DIR_OUTPUT);
    pex_set_gpio_b_low(PEX_SLP);

    // Set DIR (direction) high (normal sequence of steps in Table 4, p. 16-17)
    pex_set_dir_a(PEX_DIR, PEX_DIR_OUTPUT);
    pex_set_gpio_a_high(PEX_DIR);
    pex_set_dir_b(PEX_DIR, PEX_DIR_OUTPUT);
    pex_set_gpio_b_high(PEX_DIR);

    // Set DECAY to 0V (0% of PWM cycle is fast decay, p.14)
    pex_set_dir_a(PEX_DECAY, PEX_DIR_OUTPUT);
    pex_set_gpio_a_low(PEX_DECAY);

    // Set STEP low
    // (same STEP output for both motors A/B)
    init_cs(STEP_PIN, &STEP_DDR);
    set_cs_low(STEP_PIN, &STEP_PORT);
}

void enable_motors(void) {
    // Enable motors and disable sleep
    pex_set_gpio_a_low(PEX_EN);
    pex_set_gpio_b_low(PEX_EN);
    pex_set_gpio_a_high(PEX_SLP);
    pex_set_gpio_b_high(PEX_SLP);
}

void disable_motors(void) {
    // Disable motors and enable sleep
    pex_set_gpio_a_high(PEX_EN);
    pex_set_gpio_b_high(PEX_EN);
    pex_set_gpio_a_low(PEX_SLP);
    pex_set_gpio_b_low(PEX_SLP);
}

void actuate_motors(void) {
    print("Starting motor actuation\n");

    if (motor_fault) {
        return;
    }

    enable_motors();

    // Pulse STEP 8 times to move 45 deg (45 to 90, p. 17)
    // The motor should step on every rising edge of STEP
    for (uint8_t i = 0; i < 8; i++) {
        if (motor_fault) {
            break;
        }

        set_cs_low(STEP_PIN, &STEP_PORT);
        _delay_ms(1);
        set_cs_high(STEP_PIN, &STEP_PORT);
        _delay_ms(1);
        set_cs_low(STEP_PIN, &STEP_PORT);

        if (motor_fault) {
            break;
        }

        _delay_ms(1000);
        print("Step done\n");
    }

    disable_motors();

    print("Done motor actuation\n");
}
