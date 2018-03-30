#ifndef F_CPU
#define F_CPU 8000000UL
#endif

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

// temp
float read_temperature(void);
uint16_t read_raw_temperature(void);
float convert_temperature(uint16_t raw_temp_data);

// humidity
uint32_t read_raw_humidity(void);
double read_humidity(void);
double convert_humidity(uint32_t);
double read_humidity_temp(void);
uint16_t read_raw_humidity_temp(void);
double convert_humidity_temp(uint16_t raw_humidity_temp);

// pressure
void init_pressure_sensor(void);
void pressure_sensor_reset(void);
uint16_t read_PROM(uint8_t);
uint32_t pressure_sensor_read(uint8_t);
uint32_t read_raw_pressure(void);
uint32_t read_raw_pressure_temp(void);
float convert_pressure(uint16_t*, uint32_t, uint32_t);
