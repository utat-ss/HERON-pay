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
#define ADC1_GEN_THM            0
//ADC1 Channel 1 is GND
#define ADC1_BOOST6_VOLT_MON    2
#define ADC1_MOTOR_TEMP_1       3
#define ADC1_MOTOR_TEMP_2       4
#define ADC1_BOOST6_TEMP        5 
#define ADC1_TBOOST10_TEMP      7
#define ADC1_BOOST10_VOLT_MON   6
#define ADC1_BOOST10_CURR_MON   8
#define ADC1_BOOST6_CURR_MON    9
//ADC1 Channel 10 is GND
#define ADC1_BATT_VOLT_MON      11

#define ADC2_MF2_THM_6          0
#define ADC2_MF2_THM_5          1
#define ADC2_MF2_THM_4          2
#define ADC2_MF2_THM_3          3
#define ADC2_MF2_THM_2          4
#define ADC2_MF2_THM_1          5
#define ADC2_MF1_THM_6          7   // This is correct according to the schematic
#define ADC2_MF1_THM_5          6   // (one pair out of order) - TODO verify
#define ADC2_MF1_THM_4          8
#define ADC2_MF1_THM_3          9
#define ADC2_MF1_THM_2          10
#define ADC2_MF1_THM_1          11


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
