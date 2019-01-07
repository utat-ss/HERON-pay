#ifndef HEATERS_H
#define HEATERS_H

#include <conversions/conversions.h>
#include <dac/dac.h>
#include <dac/pay.h>

#include "devices.h"

void heaters_set_temp_a(double temp);
void heaters_set_temp_b(double temp);
void heaters_set_temp_both(double temp);

#endif
