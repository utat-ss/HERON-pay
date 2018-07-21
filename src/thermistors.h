#ifndef THERMISTORS_H
#define THERMISTORS_H

#include <stdint.h>
#include <avr/pgmspace.h>

// Resistances are stored in kilo-ohms
// PROGMEM instructs the compiler to store these values in flash memory
extern const float THERMIS_RESISTANCES[];
extern const int THERMIS_TEMPS[];
#define THERMIS_LUT_COUNT 34

#endif
