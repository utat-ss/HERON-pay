#ifndef DEVICES_H
#define DEVICES_H

#include <adc/adc.h>
#include <dac/dac.h>
#include <pex/pex.h>


// ADC CS pin
#define ADC_CS_PIN  PB6
#define ADC_CS_PORT PORTB
#define ADC_CS_DDR  DDRB

// ADC channels - for thermistors and distance sensors
#define ADC_THM_1      0
#define ADC_THM_2      1
#define ADC_THM_3      2
#define ADC_THM_4      3
#define ADC_THM_5      4
#define ADC_THM_6      5
#define ADC_THM_7      7   // This is correct according to the schematic
#define ADC_THM_8      6   // (one pair out of order) - TODO verify
#define ADC_THM_9      8
#define ADC_THM_10     9
#define ADC_POS_PHT_1  10
#define ADC_POS_PHT_2  11


// DAC CS pin
#define DAC_CS_PIN      PD0
#define DAC_CS_PORT     PORTD
#define DAC_CS_DDR      DDRD

// DAC CLR pin
#define DAC_CLR_PIN     PD1
#define DAC_CLR_PORT    PORTD
#define DAC_CLR_DDR     DDRD


// PEX CS pin
#define PEX_CS_PIN      PD5
#define PEX_CS_PORT     PORTD
#define PEX_CS_DDR      DDRD

// PEX RST pin
#define PEX_RST_PIN     PD6
#define PEX_RST_PORT    PORTD
#define PEX_RST_DDR     DDRD

// PEX address
#define PEX_ADDR        1


extern adc_t adc;
extern pex_t pex;
extern dac_t dac;

#endif
