#ifndef BOOST_H
#define BOOST_H

#include <stdbool.h>

//#include <avr/io.h>

#include <pex/pex.h>

#include "devices.h"

/* PEX pins definition */
#define BST_TEN_ENBL   0 //GPIOA0
#define BST_SIX_ENBL    7 //GPIOB7

void init_boosts(void);
void enable_10V_boost(void);
void disable_10V_boost(void);
void enable_6V_boost(void);
void disable_6V_boost(void);

#endif
