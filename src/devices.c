/*
Contains definitions of devices in PAY (e.g. ADC, PEX). This is so tests can
access the common definitions on their own without having to link to all files
that `general.c` requires.
*/

#include "devices.h"

// ADC1
pin_info_t adc1_cs = {
    .port = &ADC1_CS_PORT,
    .ddr = &ADC1_CS_DDR,
    .pin = ADC1_CS_PIN
};
adc_t adc1 = {
    .auto_channels = 0x0fff, // channels 0-11
    .cs = &adc1_cs
};

// ADC2
pin_info_t adc2_cs = {
    .port = &ADC2_CS_PORT,
    .ddr = &ADC2_CS_DDR,
    .pin = ADC2_CS_PIN
};
adc_t adc2 = {
    .auto_channels = 0x0fff, // channels 0-11
    .cs = &adc2_cs
};

// PEX
pin_info_t pex_cs = {
    .port = &PEX_CS_PORT,
    .ddr = &PEX_CS_DDR,
    .pin = PEX_CS_PIN
};
pin_info_t pex_rst = {
    .port = &PEX_RST_PORT,
    .ddr = &PEX_RST_DDR,
    .pin = PEX_RST_PIN
};
pex_t pex1 = {
    .addr = PEX1_ADDR,
    .cs = &pex_cs,
    .rst = &pex_rst
};

pex_t pex2 = {
    .addr = PEX2_ADDR,
    .cs = &pex_cs,
    .rst = &pex_rst
};
