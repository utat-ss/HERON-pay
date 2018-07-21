#ifndef HEATERS_H
#define HEATERS_H

#include "thermistors.h"
#include <dac/dac.h>

void heaters_set_temp_a(double temp);
void heaters_set_temp_b(double temp);
void heaters_set_temp_both(double temp);

#endif
