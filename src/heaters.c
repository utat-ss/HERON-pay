/*
Tests comparator functionality for heaters. Requires thermistors and heaters/ammeter
attached to payload board

Author: Brytni Richards, Bruno Almeida

For IC COMPARATOR LO PWR SGL V SOT 23-5L Part # TS391ILT
Digikey Link: https://www.digikey.ca/product-detail/en/stmicroelectronics/TS391ILT/497-2275-1-ND/599229
Datasheet: https://www.st.com/content/ccc/resource/technical/document/datasheet/de/4c/b3/3d/64/7d/48/8e/CD00001660.pdf/files/CD00001660.pdf/jcr:content/translations/en.CD00001660.pdf
*/

#include "heaters.h"

pin_info_t dac_cs = {
    .pin = DAC_CS_PIN_PAY,
    .ddr = &DAC_CS_DDR_PAY,
    .port = &DAC_CS_PORT_PAY
};

pin_info_t clr = {
    .pin = DAC_CLR_PIN_PAY,
    .ddr = &DAC_CLR_DDR_PAY,
    .port = &DAC_CLR_PORT_PAY
};

dac_t dac = {
    .cs = &dac_cs,
    .clr = &clr
};

// init is covered by dac_init()

void heaters_set_temp_a(double temp) {
    double resistance = thermis_temp_to_resistance(temp);
    double voltage = thermis_resistance_to_voltage(resistance);
    dac_set_voltage(&dac, voltage, DAC_A);
}

void heaters_set_temp_b(double temp) {
    double resistance = thermis_temp_to_resistance(temp);
    double voltage = thermis_resistance_to_voltage(resistance);
    dac_set_voltage(&dac, voltage, DAC_B);
}

void heaters_set_temp_both(double temp) {
    heaters_set_temp_a(temp);
    heaters_set_temp_b(temp);
}
