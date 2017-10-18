#include <spi/spi.h>
#include "pex.h"

// For writing to the communication register
// (when starting a communication operation with the ADC)
// to specify whether the following operation will be read or write
#define COMM_BYTE_READ  0b01000000
#define COMM_BYTE_WRITE 0b00000000
// bit[5:3] is register address
// bit[2] is CREAD. Set to 1 to enable continuous read

// Register addresses (p.20)
#define STATUS_ADDR     0x00
#define MODE_ADDR       0x01
#define CONFIG_ADDR     0x02
#define DATA_ADDR       0x03
#define ID_ADDR         0x04
#define GPOCON_ADDR     0x05
#define OFFSET_ADDR     0x06
#define FULL_SCALE_ADDR 0x07

#define CONFIG_DEFAULT 0x040008

// GPIOB pins on the PAY_SENSOR board's port expander
#define ADC_CS 0
#define ITF_CS 1