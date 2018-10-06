/*
Functions and data for measuring thermistor temperature

AUTHORS: Dylan Vogel, James Sheridan, Bruno Almeida

// For NTC Thermistor 10k 0603 (1608 Metric) Part # NCU18XH103F60RB
// Digikey link: https://www.digikey.ca/product-detail/en/murata-electronics-north-america/NCU18XH103F60RB/490-16279-1-ND/7363262
// Datasheet (page 13. Part # NCU__XH103):
// https://www.murata.com/~/media/webrenewal/support/library/catalog/products/thermistor/r03e.ashx?la=en-us
// Datasheet (NCU18XH103F60RB):
// https://www.murata.com/en-us/api/pdfdownloadapi?cate=luNTCforTempeSenso&partno=NCU18XH103F60RB

TODO: Implement more efficient channel reading algorithm in lib-common ADC library,
      instead of having to waste SPI frames from waiting for
      next message
TODO: Test edge cases
*/

#include "thermistors.h"

// TODO - create test to verify PROGMEM values

// Lookup table from manufacturer datasheet (pg 13)
// TODO - confirm thermistor part number

// Resistances are stored in kilo-ohms
// PROGMEM instructs the compiler to store these values in flash memory
const float THERMIS_RESISTANCES[THERMIS_LUT_COUNT] PROGMEM = {
    195.652,    148.171,    113.347,    87.559,     68.237,
    53.650,     42.506,     33.892,     27.219,     22.021,
    17.926,     14.674,     12.081,     10.000,     8.315,
    6.948,      5.834,      4.917,      4.161,      3.535,
    3.014,      2.586,      2.228,      1.925,      1.669,
    1.452,      1.268,      1.110,      0.974,      0.858,
    0.758,      0.672,      0.596,      0.531
};

// Temperatures in C
const int THERMIS_TEMPS[THERMIS_LUT_COUNT] PROGMEM = {
    -40,        -35,        -30,        -25,        -20,
    -15,        -10,        -5,         0,          5,
    10,         15,         20,         25,         30,
    35,         40,         45,         50,         55,
    60,         65,         70,         75,         80,
    85,         90,         95,         100,        105,
    110,        115,        120,        125
};


// Convert the measured thermistor (NXRT15XH103FA1B040) resistance to temperature
double thermis_resistance_to_temp(double resistance){
    for (uint8_t i = 0; i < THERMIS_LUT_COUNT - 1; i++){
        double resistance_next = pgm_read_float(&THERMIS_RESISTANCES[i + 1]);

        if ((resistance - resistance_next) >= 0){
            double resistance_prev = pgm_read_float(&THERMIS_RESISTANCES[i]);
            int16_t temp_next = pgm_read_word(&THERMIS_TEMPS[i + 1]);
            int16_t temp_prev = pgm_read_word(&THERMIS_TEMPS[i]);

            double diff = resistance - resistance_prev;  //should be negative
            double slope = ((double) (temp_next - temp_prev)) / (resistance_next - resistance_prev);
            return temp_prev + (diff * slope);
        }
    }

    // TODO - this shouldn't happen
    return 0.0;
}

// Convert the thermistor temperature to resistance
double thermis_temp_to_resistance(double temp) {
    for (uint8_t i = 0; i < THERMIS_LUT_COUNT - 1; i++){
        int16_t temp_next = pgm_read_word(&THERMIS_TEMPS[i + 1]);

        if ((temp - temp_next) <= 0){
            int16_t temp_prev = pgm_read_word(&THERMIS_TEMPS[i]);
            double resistance_next = pgm_read_float(&THERMIS_RESISTANCES[i + 1]);
            double resistance_prev = pgm_read_float(&THERMIS_RESISTANCES[i]);

            double diff = temp - temp_prev;  //should be negative
            double slope = (resistance_next - resistance_prev) / ((double) (temp_next - temp_prev));
            return resistance_prev + (diff * slope);
        }
    }

    // TODO - this shouldn't happen
    return 0.0;
}

// Get the voltage at the point between the thermistor and the constant 10k resistor
// (10k connected to ground)
// See: https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
double thermis_resistance_to_voltage(double resistance) {
    return THERMIS_V_REF * THERMIS_R_REF / (resistance + THERMIS_R_REF);
}
