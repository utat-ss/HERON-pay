#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <spi/spi.h>

// Thermistor ADC is using COM_RST to bypass the port expander
#define COM_RST         PC6
#define COM_RST_DDR     DDRC
#define COM_RST_PORT    PORTC

#define THERMISTOR_BASE 0b10001100
#define R_REF   9.1            // KOhm

void init_thermistor(void);
uint16_t read_thermistor_adc(int channel);
double convert_thermistor_reading(uint16_t adc_reading);
double resistance_to_temp(double res);
