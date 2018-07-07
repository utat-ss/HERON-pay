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
static const uint16_t R_REF[4] = {9010, 9960, 9980, 9950};  // (Ohm); measured value

void init_thermistor(void);
uint16_t read_thermistor_adc(int channel);
double convert_thermistor_reading(uint16_t adc_reading, int channel);
double resistance_to_temp(double res);
