#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <spi/spi.h>

// Thermistor ADC is using COM_RST to bypass the port expander
#define COM_RST         PC6
#define COM_RST_DDR     DDRC
#define COM_RST_PORT    PORTC

// Refer to page 9 of the datasheet for bit configuration
#define THERMISTOR_BASE 0b10000100
#define R_REF   9.1            // (KOhm); measured value

void init_thermistor(void);
uint16_t read_thermistor_adc(int channel);
double convert_thermistor_reading(uint16_t adc_reading);
double resistance_to_temp(double res);
