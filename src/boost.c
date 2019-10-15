/*
Functions called for enabling/disabling two boost converters on pay-ssm v4

Boost converter: http://www.ti.com/lit/ds/symlink/tps61088.pdf
*/

#include "boost.h"

void init_boosts(void) {
    // set 10V boost associated pins
    set_pex_pin_dir(&pex2, PEX_A, BST_TEN_ENBL, OUTPUT);
    set_pex_pin(&pex2, PEX_A, BST_TEN_ENBL, 0);

    // set 6V boost associated pins
    set_pex_pin_dir(&pex2, PEX_B, BST_SIX_ENBL, OUTPUT);
    set_pex_pin(&pex2, PEX_B, BST_SIX_ENBL, 0);
}

void enable_10V_boost(void) {
    // Enable 10V boost converter by setting corresponding pex pin HIGH
    set_pex_pin(&pex2, PEX_A, BST_TEN_ENBL, 1);
}

void disable_10V_boost(void) {
    // Disable 10V boost converter by setting corresponding pex pin HIGH
    set_pex_pin(&pex2, PEX_A, BST_TEN_ENBL, 0);
}

void enable_6V_boost(void) {
    // Enable 6V boost converter by setting corresponding pex pin HIGH
    set_pex_pin(&pex2, PEX_B, BST_SIX_ENBL, 1);
}

void disable_6V_boost(void) {
    // Disable 6V boost converter by setting corresponding pex pin HIGH
    set_pex_pin(&pex2, PEX_B, BST_SIX_ENBL, 0);
}
