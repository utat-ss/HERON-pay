#ifndef THERMISTORS_H
#define THERMISTORS_H

#include <stdint.h>

#include <avr/pgmspace.h>

#define THERMIS_V_REF     2.5
#define THERMIS_R_REF 10.0 // when referencing table in thermistors.c, values are in kilo Ohmns

// Resistances are stored in kilo-ohms
// PROGMEM instructs the compiler to store these values in flash memory
extern const float THERMIS_RESISTANCES[];
extern const int THERMIS_TEMPS[];
#define THERMIS_LUT_COUNT 34

double thermis_resistance_to_temp(double resistance);
double thermis_temp_to_resistance(double temp);
double thermis_resistance_to_voltage(double resistance);

#endif
