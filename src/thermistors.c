/*
Functions and data for measuring thermistor temperature

AUTHORS: Dylan Vogel, James Sheridan, Bruno Almeida

TODO: Implement more efficient channel reading algorithm in lib-common ADC library,
      instead of having to waste SPI frames from waiting for
      next message
TODO: Test edge cases
*/

#include "thermistors.h"

// TODO - create test to verify PROGMEM values

// Lookup table from manufacturer datasheet (pg 31)
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


// TODO
// // Converts the ADC reading into a thermistor resistance
// // Assumes the thermistor is wired in series with a resistor of resistance
// // R_REF, using the 2.5V ADC reference voltage
// // See: https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
// double convert_thermistor_reading(uint16_t adc_reading, int channel){
//     // Equation is as follows, after rearringing the voltage divider equation:
//     // R = R0 [(ADC_max_value / ADC_read_value) - 1]
//
//     double res;
//
//     res = (double)(1<<12) / adc_reading;
//     res = (double)(R_REF[channel]) / 1000.0 * (res - 1.0);
//
//     return res;
// }

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
        double temp_next = pgm_read_word(&THERMIS_TEMPS[i + 1]);

        if ((temp - temp_next) <= 0){
            double temp_prev = pgm_read_word(&THERMIS_TEMPS[i]);
            int16_t resistance_next = pgm_read_float(&THERMIS_RESISTANCES[i + 1]);
            int16_t resistance_prev = pgm_read_float(&THERMIS_RESISTANCES[i]);

            double diff = temp - temp_prev;  //should be negative
            double slope = ((double) (resistance_next - resistance_prev)) / (temp_next - temp_prev);
            return resistance_prev + (diff * slope);
        }
    }

    // TODO - this shouldn't happen
    return 0.0;
}
