#ifndef HEATERS_H
#define HEATERS_H

#include "thermistors.h"
#include <dac/dac.h>
#include <dac/pay.h>

extern dac_t dac;
extern pin_info_t dac_cs;
extern pin_info_t clr;

void heaters_set_temp_a(double temp);
void heaters_set_temp_b(double temp);
void heaters_set_temp_both(double temp);

#endif
