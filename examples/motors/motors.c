#include "motors.h"

// PEX
pin_info_t pex_cs = {
    .port = &PEX_CS_PORT,
    .ddr = &PEX_CS_DDR,
    .pin = PEX_CS_PIN
};
pin_info_t pex_rst = {
    .port = &PEX_RST_PORT,
    .ddr = &PEX_RST_DDR,
    .pin = PEX_RST_PIN
};
pex_t pex = {
    .addr = PEX_ADDR,
    .cs = &pex_cs,
    .rst = &pex_rst
};

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
    // nRESET = 1 for no reset
    //init_output_pin(RESET_M2, &MOT_RESET_DDR, 1);
    //init_output_pin(RESET_M1, &MOT_RESET_DDR, 1);

    // nSLEEP = 0 - disable
    set_pex_pin_dir(&pex, PEX_A, PEX_SLP_M2, OUTPUT);
    set_pex_pin_dir(&pex, PEX_B, PEX_SLP_M1, OUTPUT);
    set_pex_pin(&pex, PEX_A, PEX_SLP_M1, 0);
    set_pex_pin(&pex, PEX_B, PEX_SLP_M2, 0);

    // DECAY = 1 (fast decay)
    // TODO - might need to connect BDECAY high in hardware (currently tied to ground)
    set_pex_pin_dir(&pex, PEX_A, PEX_DECAY_M2, OUTPUT);
    set_pex_pin_dir(&pex, PEX_B, PEX_DECAY_M1, OUTPUT);
    set_pex_pin(&pex, PEX_A, PEX_DECAY_M2, 1);
    set_pex_pin(&pex, PEX_B, PEX_DECAY_M1, 1);

    // Set no current through bridges
    // AI0/BI0/AI1/BI1 = 0
    set_pex_pin_dir(&pex, PEX_A, PEX_AI0_M2, OUTPUT);
    set_pex_pin_dir(&pex, PEX_A, PEX_AI1_M2, OUTPUT);
    set_pex_pin_dir(&pex, PEX_A, PEX_BI0_M2, OUTPUT);
    set_pex_pin_dir(&pex, PEX_A, PEX_BI1_M2, OUTPUT);
    set_pex_pin(&pex, PEX_A, PEX_AI0_M2, 1);
    set_pex_pin(&pex, PEX_A, PEX_AI1_M2, 1);
    set_pex_pin(&pex, PEX_A, PEX_BI0_M2, 1);
    set_pex_pin(&pex, PEX_A, PEX_BI1_M2, 1);

    set_pex_pin_dir(&pex, PEX_B, PEX_AI0_M1, OUTPUT);
    set_pex_pin_dir(&pex, PEX_B, PEX_AI1_M1, OUTPUT);
    set_pex_pin_dir(&pex, PEX_B, PEX_BI0_M1, OUTPUT);
    set_pex_pin_dir(&pex, PEX_B, PEX_BI1_M1, OUTPUT);
    set_pex_pin(&pex, PEX_B, PEX_AI0_M1, 1);
    set_pex_pin(&pex, PEX_B, PEX_AI1_M1, 1);
    set_pex_pin(&pex, PEX_B, PEX_BI0_M1, 1);
    set_pex_pin(&pex, PEX_B, PEX_BI1_M1, 1);

    // Logic output pins motor 1
    init_output_pin(AIN1_M1, &DDR_M1, 0);
    init_output_pin(AIN2_M1, &DDR_M1, 0);
    init_output_pin(BIN1_M1, &DDR_M1, 0);
    init_output_pin(BIN2_M1, &DDR_M1, 0);

    // Logic output pins motor 2
    init_output_pin(AIN1_M2, &DDR_M2, 0);
    init_output_pin(AIN2_M2, &DDR_M2, 0);
    init_output_pin(BIN1_M2, &DDR_M2, 0);
    init_output_pin(BIN2_M2, &DDR_M2, 0);

}

void enable_motors(void) {
    // Disable sleep

    // nSLEEP = 1
    set_pex_pin(&pex, PEX_A, PEX_SLP_M1, 1);
    set_pex_pin(&pex, PEX_B, PEX_SLP_M2, 1);

    // 100% Current through H bridges
    set_pex_pin(&pex, PEX_A, PEX_AI0_M2, 0);
    set_pex_pin(&pex, PEX_A, PEX_AI1_M2, 0);
    set_pex_pin(&pex, PEX_A, PEX_BI0_M2, 0);
    set_pex_pin(&pex, PEX_A, PEX_BI1_M2, 0);

    set_pex_pin(&pex, PEX_B, PEX_AI0_M1, 0);
    set_pex_pin(&pex, PEX_B, PEX_AI1_M1, 1);
    set_pex_pin(&pex, PEX_B, PEX_BI0_M1, 0);
    set_pex_pin(&pex, PEX_B, PEX_BI1_M1, 1);
}

void disable_motors(void) {
    // Disable motors and enable sleep

    // nSLEEP = 0
    set_pex_pin(&pex, PEX_A, PEX_SLP_M1, 0);
    set_pex_pin(&pex, PEX_B, PEX_SLP_M2, 0);
}

void reset_motors(void) {
    // Reset motors
    set_pin_low(RESET_M1, &MOT_RESET_DDR);
    set_pin_low(RESET_M2, &MOT_RESET_DDR);

    init_motors();
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
            // BOUT1 = 1, BOUT2 = 0
            delay_ms(delay);
            set_pin_high(BIN1_M2, &PORT_M2);
            set_pin_high(BIN1_M1, &PORT_M1);
            set_pin_low(BIN2_M2, &PORT_M2);
            set_pin_low(BIN2_M1, &PORT_M1);

            // APHASE = 1
            // AOUT1 = 1, AOUT2 = 0
            delay_ms(delay);
            set_pin_high(AIN1_M2, &PORT_M2);
            set_pin_high(AIN1_M1, &PORT_M1);
            set_pin_low(AIN2_M2, &PORT_M2);
            set_pin_low(AIN2_M1, &PORT_M1);

            // BPHASE = 0
            // BOUT1 = 0, BOUT2 = 1
            delay_ms(delay);
            set_pin_low(BIN1_M2, &PORT_M2);
            set_pin_low(BIN1_M1, &PORT_M1);
            set_pin_high(BIN2_M2, &PORT_M2);
            set_pin_high(BIN2_M1, &PORT_M1);

            // APHASE = 0
            // AOUT1 = 0, AOUT2 = 1
            delay_ms(delay);
            set_pin_low(AIN1_M2, &PORT_M2);
            set_pin_low(AIN1_M1, &PORT_M1);
            set_pin_high(AIN2_M2, &PORT_M2);
            set_pin_high(AIN2_M1, &PORT_M1);
        }
        else {
            // APHASE = 1
            // AOUT1 = 1, AOUT2 = 0
            delay_ms(delay);
            set_pin_high(AIN1_M2, &PORT_M2);
            set_pin_high(AIN1_M1, &PORT_M1);
            set_pin_low(AIN2_M2, &PORT_M2);
            set_pin_low(AIN2_M1, &PORT_M1);

            // BPHASE = 1
            // BOUT1 = 1, BOUT2 = 0
            delay_ms(delay);
            set_pin_high(BIN1_M2, &PORT_M2);
            set_pin_high(BIN1_M1, &PORT_M1);
            set_pin_low(BIN2_M2, &PORT_M2);
            set_pin_low(BIN2_M1, &PORT_M1);

            // APHASE = 0
            // AOUT1 = 0, AOUT2 = 1
            delay_ms(delay);
            set_pin_low(AIN1_M2, &PORT_M2);
            set_pin_low(AIN1_M1, &PORT_M1);
            set_pin_high(AIN2_M2, &PORT_M2);
            set_pin_high(AIN2_M1, &PORT_M1);

            // BPHASE = 0
            // BOUT1 = 0, BOUT2 = 1
            delay_ms(delay);
            set_pin_low(BIN1_M2, &PORT_M2);
            set_pin_low(BIN1_M1, &PORT_M1);
            set_pin_high(BIN2_M2, &PORT_M2);
            set_pin_high(BIN2_M1, &PORT_M1);
        }
    }
    disable_motors();
}

/*
ISR(INT2_vect) {
    print("INT2 - Motor Fault (PEX INTA)\n");

    // Check if either of the motor FLTn (fault) pins is low
    if (get_pin_val(FAULT_M1, &FAULT_M1_PORT) == 0) {
        motor_fault = true;
        print("MOTOR 1\n");
    }
    if (get_pin_val(FAULT_M2, &FAULT_M2_PORT)  == 0) {
        motor_fault = true;
        print("MOTOR 2\n");
    }

    if (motor_fault) {
        disable_motors();
        // Maybe change this to reset motors
    }
}*/

int main(void){
    init_uart();
    init_spi();
    init_pex(&pex);
    print("Pex and Uart initialized\n");

    init_motors();
    print("Pins Motors initialized\n");

    enable_motors();
    print("motors enabled\n");

    //actuate_motors(100, 100, true);

    delay_ms(5);
    set_pin_low(AIN2_M2, &PORT_M2);
    set_pin_low(AIN2_M1, &PORT_M1);
    set_pin_low(BIN2_M2, &PORT_M2);
    set_pin_low(BIN2_M1, &PORT_M1);
    while(1){
        delay_ms(1);
        set_pin_high(AIN1_M2, &PORT_M2);
        set_pin_high(AIN1_M1, &PORT_M1);
        set_pin_high(BIN1_M2, &PORT_M2);
        set_pin_high(BIN1_M1, &PORT_M1);
        delay_ms(1);
        set_pin_low(AIN1_M2, &PORT_M2);
        set_pin_low(AIN1_M1, &PORT_M1);
        set_pin_low(BIN1_M2, &PORT_M2);
        set_pin_low(BIN1_M1, &PORT_M1);
    }
}
