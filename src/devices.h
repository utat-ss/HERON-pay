#ifndef DEVICES_H
#define DEVICES_H

#include <adc/adc.h>
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
#define ADC1_BOOST10_TEMP       6
#define ADC1_BOOST10_VOLT_MON   7
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

// Resistor divider values (for voltage monitors)
// Sense resistor and voltage reference values (for current monitors)
#define ADC1_BOOST10_LOW_RES    1000
#define ADC1_BOOST10_HIGH_RES   3000
#define ADC1_BOOST10_SENSE_RES  0.008
#define ADC1_BOOST10_REF_VOL    0.0
#define ADC1_BOOST6_LOW_RES     1000
#define ADC1_BOOST6_HIGH_RES    1400
#define ADC1_BOOST6_SENSE_RES   0.008
#define ADC1_BOOST6_REF_VOL     0.0
#define ADC1_BATT_LOW_RES       2200
#define ADC1_BATT_HIGH_RES      1000

// PEX CS pin
#define PEX_CS_PIN      PC4
#define PEX_CS_PORT     PORTC
#define PEX_CS_DDR      DDRC

// PEX RST pin
#define PEX_RST_PIN     PC5
#define PEX_RST_PORT    PORTC
#define PEX_RST_DDR     DDRC

// PEX address
#define PEX1_ADDR       0b001
#define PEX2_ADDR       0b010


extern adc_t adc1;
extern adc_t adc2;
extern pex_t pex1;
extern pex_t pex2;


#endif
