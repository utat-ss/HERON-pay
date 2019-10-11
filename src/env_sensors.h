#ifndef ENV_SENSORS_H
#define ENV_SENSORS_H

#include <stdint.h>

#include <avr/io.h>

#include <spi/spi.h>
#include <uart/uart.h>

/* Humidity Sensor */

#define HUM_CS_PIN  PD0
#define HUM_CS_PORT PORTD
#define HUM_CS_DDR  DDRD

void init_hum(void);
uint16_t read_hum_raw_data(void);


/* Pressure Sensor */

#define PRES_CS_PIN     PC1
#define PRES_CS_PORT    PORTC
#define PRES_CS_DDR     DDRC

// Commands
#define PRES_CMD_RESET          0x1E
#define PRES_CMD_D1_4096        0x48
#define PRES_CMD_D2_4096        0x58
#define PRES_CMD_ADC_READ       0x00
#define PRES_CMD_PROM_READ_BASE 0xA0   // to 0xAE

void init_pres(void);
void reset_pres(void);
uint16_t read_pres_prom(uint8_t address);
uint32_t read_pres_raw_uncomp_data(uint8_t cmd);
uint32_t pres_reg_data_to_raw_data(
    uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6,
    uint32_t D1, uint32_t D2);
uint32_t read_pres_raw_data(void);

#endif
