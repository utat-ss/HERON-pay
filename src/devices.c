/*
Contains definitions of devices in PAY (e.g. ADC, PEX). This is so tests can
access the common definitions on their own without having to link to all files
that `general.c` requires.
*/

#include "devices.h"

// ADC
pin_info_t adc_cs = {
    .port = &ADC_CS_PORT_PAY,
    .ddr = &ADC_CS_DDR_PAY,
    .pin = ADC_CS_PIN_PAY
};
adc_t adc = {
    .channels = 0x0fff, // channels 0-11
    .cs = &adc_cs
};

// PEX
pin_info_t pex_cs = {
    .port = &PEX_CS_PORT_PAY,
    .ddr = &PEX_CS_DDR_PAY,
    .pin = PEX_CS_PIN_PAY
};
pin_info_t pex_rst = {
    .port = &PEX_RST_PORT_PAY,
    .ddr = &PEX_RST_DDR_PAY,
    .pin = PEX_RST_PIN_PAY
};
pex_t pex = {
    .addr = PEX_ADDR_PAY,
    .cs = &pex_cs,
    .rst = &pex_rst
};

// DAC
pin_info_t dac_cs = {
    .pin = DAC_CS_PIN_PAY,
    .ddr = &DAC_CS_DDR_PAY,
    .port = &DAC_CS_PORT_PAY
};
pin_info_t dac_clr = {
    .pin = DAC_CLR_PIN_PAY,
    .ddr = &DAC_CLR_DDR_PAY,
    .port = &DAC_CLR_PORT_PAY
};
dac_t dac = {
    .cs = &dac_cs,
    .clr = &dac_clr,
    .raw_voltage_a = 0,
    .raw_voltage_b = 0
};
