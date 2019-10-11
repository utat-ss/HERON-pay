#ifndef BOOST_H
#define BOOST_H

#include <stdbool.h>

#include <avr/io.h>

#include <pex/pex.h>
#include <uart/uart.h>

#include "devices.h"

/* PEX pins definition */
#define 10VBST_ENBL   0 //GPIOA0
#define 6VBST_ENBL    7 //GPIOB7

void init_boosts(void);
void enable_10V_boost(void);
void disable_10V_boost(void);
void enable_6V_boost(void);
void disable_6V_boost(void);

#endif
