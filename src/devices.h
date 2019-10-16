#ifndef DEVICES_H
#define DEVICES_H

#include <adc/adc.h>
#include <dac/dac.h>
#include <pex/pex.h>


// ADC CS pin
#define ADC1_CS_PIN  PB5
#define ADC1_CS_PORT PORTB
#define ADC1_CS_DDR  DDRB

#define ADC2_CS_PIN  PB6
#define ADC2_CS_PORT PORTB
#define ADC2_CS_DDR  DDRB

// ADC1 channels - for general ambient temperature, two boost states, battery voltage
// channel 1 and 10 unused
#define ADC1_GEN_THM            0
#define ADC1_BOOST6_VOLT_MON    2
#define ADC1_MOTOR_TEMP_1       3
#define ADC1_MOTOR_TEMP_2       4
#define ADC1_BOOST6_TEMP        5
#define ADC1_TBOOST10_TEMP      7
#define ADC1_BOOST10_VOLT_MON   6
#define ADC1_BOOST10_CURR_MON   8
#define ADC1_BOOST6_CURR_MON    9
#define ADC1_BATT_VOLT_MON      11

// ADC2 channels - dedicated to MF chip thermistors readings
#define ADC2_MF2_THM_6          0
#define ADC2_MF2_THM_5          1
#define ADC2_MF2_THM_4          2
#define ADC2_MF2_THM_3          3
#define ADC2_MF2_THM_2          4
#define ADC2_MF2_THM_1          5
#define ADC2_MF1_THM_6          6
#define ADC2_MF1_THM_5          7
#define ADC2_MF1_THM_4          8
#define ADC2_MF1_THM_3          9
#define ADC2_MF1_THM_2          10
#define ADC2_MF1_THM_1          11


// PEX CS pin
#define PEX_CS_PIN      PC4
#define PEX_CS_PORT     PORTC
#define PEX_CS_DDR      DDRC

// PEX RST pin
#define PEX_RST_PIN     PC5
#define PEX_RST_PORT    PORTC
#define PEX_RST_DDR     DDRC

// PEX address
#define PEX1_ADDR        4
#define PEX2_ADDR        2


extern adc_t adc1;
extern adc_t adc2;
extern pex_t pex1;
extern pex_t pex2;


#endif
