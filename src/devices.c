/*
Contains definitions of devices in PAY (e.g. ADC, PEX). This is so tests can
access the common definitions on their own without having to link to all files
that `general.c` requires.
*/

#include "devices.h"

// ADC
pin_info_t adc_cs = {
    .port = &ADC_CS_PORT,
    .ddr = &ADC_CS_DDR,
    .pin = ADC_CS_PIN
};
adc_t adc = {
    .auto_channels = 0x0fff, // channels 0-11
    .cs = &adc_cs
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
pex_t pex = {
    .addr = PEX_ADDR,
    .cs = &pex_cs,
    .rst = &pex_rst
};
