#include <stdint.h>
#include <uart/log.h>

#ifndef _can_handler_h
#define _can_handler_h

void pay_can_handler(uint16_t identifier, uint8_t* pt_data, uint8_t size);

#endif