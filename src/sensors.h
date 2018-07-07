#ifndef SENSORS_H
#define SENSORS_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <spi/spi.h>


#define PRES_RESET_SPI  0x1E
#define PRES_D1_4096    0x48
#define PRES_D2_4096    0x58
#define PRES_ADC_READ   0x00
#define PRES_PROM_BASE  0xA0

// Temperature sensor
#define TEMP_CS_PIN     PC5
#define TEMP_CS_PORT    PORTC
#define TEMP_CS_DDR     DDRC

// TODO - what is THM_DIS/TM_DIS??
// shutdown mode?

// Humidity sensor
#define HUM_CS_PIN  PC4
#define HUM_CS_PORT PORTC
#define HUM_CS_DDR  DDRC

// Pressure sensor
#define PRES_CS_PIN     PC6
#define PRES_CS_PORT    PORTC
#define PRES_CS_DDR     DDRC

// Optical ADC - read conversion (p.31)
#define ADC_OPTICAL_N       24  // number of bits read
#define ADC_OPTICAL_V_REF   2.5 // reference voltage




// temperature
void temp_init(void);
uint16_t temp_read_raw_data(void);

// humidity
void hum_init(void);
uint32_t hum_read_raw_data(void);
uint16_t hum_read_raw_humidity(void);
uint16_t hum_read_raw_temperature(void);
double hum_convert_raw_temperature_to_temperature(uint16_t raw_temperature);

// pressure
void pres_init(void);
void pres_reset(void);
uint16_t pres_read_prom(uint8_t address);
uint32_t pres_read_raw_uncompensated_data(uint8_t cmd);
uint32_t pres_read_raw_uncompensated_pressure(void);
uint32_t pres_read_raw_uncompensated_temperature(void);
uint32_t pres_convert_raw_uncompensated_data_to_raw_pressure(uint32_t D1, uint32_t D2, uint32_t *raw_temperature);
uint32_t pres_read_raw_pressure(void);
float pres_convert_raw_temperature_to_temperature(uint32_t raw_temperature);

#endif
