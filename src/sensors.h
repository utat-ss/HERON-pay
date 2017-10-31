/*#ifndef F_CPU
#define F_CPU 8000000UL
#endif*/

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <spi/spi.h>

#include "pex.h"

#define RESET 0x1E
#define D1_4096 0x48
#define D2_4096 0x58
#define ADC_READ 0x00
#define PROM_BASE 0xA0

uint16_t PROM_data[8];

void sensor_setup(void);

float read_temperature(void);
uint32_t get_raw_humidity(void);
double read_humidity(void);
double read_humidity_temp(void);

void pressure_sensor_reset(void);
uint16_t read_PROM(uint8_t address);
uint32_t pressure_sensor_read(uint8_t);
float read_pressure(uint16_t*);
