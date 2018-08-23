#ifndef SENSORS_H
#define SENSORS_H

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <spi/spi.h>
#include <uart/uart.h>




/* Temperature Sensor */

#define TEMP_CS_PIN     PC5
#define TEMP_CS_PORT    PORTC
#define TEMP_CS_DDR     DDRC

// TODO - what is THM_DIS/TM_DIS?

void temp_init(void);
uint16_t temp_read_raw_data(void);




/* Humidity Sensor */

#define HUM_CS_PIN  PC4
#define HUM_CS_PORT PORTC
#define HUM_CS_DDR  DDRC

void hum_init(void);
uint16_t hum_read_raw_data(void);




/* Pressure Sensor */

#define PRES_CS_PIN     PC6
#define PRES_CS_PORT    PORTC
#define PRES_CS_DDR     DDRC

// Commands
#define PRES_CMD_RESET          0x1E
#define PRES_CMD_D1_4096        0x48
#define PRES_CMD_D2_4096        0x58
#define PRES_CMD_ADC_READ       0x00
#define PRES_CMD_PROM_READ_BASE 0xA0   // to 0xAE

void pres_init(void);
void pres_reset(void);
uint16_t pres_read_prom(uint8_t address);
uint32_t pres_read_raw_uncomp_data(uint8_t cmd);
uint32_t pres_reg_data_to_raw_data(
    uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6,
    uint32_t D1, uint32_t D2);
uint32_t pres_read_raw_data(void);

#endif
