/*

	FILENAME: 			adc.c
	DEPENDENCIES:		pex, spi

	DESCRIPTION:		AD7194 ADC functions
	AUTHORS:			Bruno Almeida, Dylan Vogel
	DATE MODIFIED:	    2017-12-28
	NOTES:

                  TODO:
                    * Consider implementing CS is hold low for conversion process
                    * Add a function to automatically scale gain
                    * Add calibration function
                    * Adapt to handle multiple ADCs
                    * Fix the CS and hardware address issues

                  Configuration register bits:
                    * bit[23] to 0 - chop bit
                    * bit[20] set to 0 — ref select
                    * bit[18] should be 1 (psuedo bit)
                    * bit[16] set to 0 — temp bit
                    * bit[15:8] – channel select
                    * bit[7] set to 0 – burnout
                    * bit[6] set to 0 - ref detect
                    * bit[4] set to 0 – buffer, might want to enable
                    * bit[3] set to 1 - unipolar or bipolar (1 for unipolar)
                    * bit[2:0] set to 000 - programmable gain


	REVISION HISTORY:

        2017-12-28:     DV: Reverted BioStack I changes to read_ADC_channel.
                        BA: implemented calibrate_adc();
        2017-11-17:     Fixed bit shift in channel selection function from 8 to 12
		2017-11-16: 	Created header. Existing functions created by Bruno and Dylan

*/

#include "adc.h"

void init_adc(void){
  // Initialize ports and registers needed for ADC usage
  // ASSUMES init_port_expander() has already been called

  int i;

  // Set up LEDs as output and set to low
  for (i = 0; i < 4; i++){
    set_dir_a(SENSOR_PCB, i, 0);
    clear_gpio_a(SENSOR_PCB, i);
  }
  // Set ITF and ADC CS as output and set to high
  for (i = 0; i < 2; i++){
    set_gpio_b(SENSOR_PCB, i);
    set_dir_b(SENSOR_PCB, i, 0);
    //set_gpio_b(SENSOR_PCB, i);
  }

  // NOTE: The following line is only disabled while 'hard' PEX reset is being called
  //        We wish to maintain ADC config states between PEX resets
  // write_ADC_register(CONFIG_ADDR, CONFIG_DEFAULT);
  // "Continuous conversion is the default power-up mode." (p. 32)
}

void adc_pex_hard_rst(void){
  // Perform a hard reset of the ADC port expander
  // Toggle PEX RST pin and re-initialize the the default state

  // BUG:  Current issue, it will reset all connected port expanders
  //        Make it not do this.

  reset_pex();

  // BUG: Port expandes reset to address 000 on reset.
  //      To write to them, need to write to 000
  //      This is the address of the SSM PEX. Therefore need to change SSM PEX hardware address
  port_expander_write(0x00, IOCON, IOCON_DEFAULT);
  init_adc();
}


uint32_t read_ADC_register(uint8_t register_addr) {
  // Read the current state of the specified ADC register.

  clear_gpio_b(SENSOR_PCB, ADC_CS);
  send_spi(COMM_BYTE_READ_SINGLE | (register_addr << 3));

  // Read the required number of bytes based on register
  uint32_t data = 0;
  for (int i = 0; i < num_register_bytes(register_addr); i++) {
    data = data << 8;
    data = data | send_spi(0);
  }

  // BUG: Shouldn't 'hard reset' the PEX in final implementation
  adc_pex_hard_rst();

  // Set CS high
  //set_gpio_b(SENSOR_PCB, ADC_CS);

  return data;
}

void write_ADC_register(uint8_t register_addr, uint32_t data) {
  // Writes a new state to the specified ADC register.

  clear_gpio_b(SENSOR_PCB, ADC_CS);
  send_spi(COMM_BYTE_WRITE | (register_addr << 3));

  // Write the number of bytes in the register
  for (int i = num_register_bytes(register_addr) - 1; i >= 0; i--) {
    send_spi( (uint8_t)(data >> (i * 8)) );
  }

  // BUG: Shouldn't 'hard reset' the PEX in final implementation
  adc_pex_hard_rst();

  // Set CS high
  //set_gpio_b(SENSOR_PCB, ADC_CS);
}

uint8_t num_register_bytes(uint8_t register_addr) {
  // Returns the number of BYTES in the specified register at the given address
  // TODO - careful of data register + status information - pg. 20

  switch (register_addr) {
    case STATUS_ADDR:
      return 1;
      break;
    case MODE_ADDR:
      return 3;
      break;
    case CONFIG_ADDR:
      return 3;
      break;
    case DATA_ADDR:
      return 3;
      break;
    case ID_ADDR:
      return 1;
      break;
    case GPOCON_ADDR:
      return 1;
      break;
    case OFFSET_ADDR:
      return 3;
      break;
    case FULL_SCALE_ADDR:
      return 3;
      break;
    default:
      return 0;
      break;
  }
}

void select_ADC_channel(uint8_t channel_num) {
  // Sets the configuration register's bits for the specified ADC input channel.
  // channel_num - one of 5, 6, 7

  // Get the 4 bits for the channel (p. 26)
  uint8_t channel_bits = channel_num - 1;

  // Read the current register status
  uint32_t config_data = read_ADC_register(CONFIG_ADDR);

  // Mask the channel bits and write new channel
  config_data &= 0xffff00ff;
  config_data |= (channel_bits << 12);

  // Write modified config register
  write_ADC_register(CONFIG_ADDR, config_data);
}

uint32_t read_ADC_channel(uint8_t channel_num) {
  // Read 24 bit raw data from the specified ADC channel.
  // channel_num - one of 5, 6, 7

  uint32_t read_data;

  // Select the channel for conversion
  select_ADC_channel(channel_num);

  // Wait until the conversion finishes, signalled by MISO going high
  // BUG: conversion is finished when MISO goes *LOW* - fixed
  // TODO: add a timeout
  while (bit_is_set(PINB, PB0)){
      continue;
  }

  // Read back the conversion result
  read_data = read_ADC_register(DATA_ADDR);

  return read_data;
}


double convert_ADC_reading(uint32_t ADC_reading, uint8_t pga_gain) {
  // Reads the input voltage for the specified ADC channel,
  // including applying the gain factor.
  // channel_num - one of 5, 6, 7


  // (p.31) Code = (2^N * AIN * Gain) / (V_REF)
  //     => AIN = (Code * V_REF) / (2^N * Gain)
  double num = ADC_reading * V_REF;
  double denom = (((uint32_t) 1) << N) * pga_gain;
  double AIN = num / denom;

  // return AIN;
  return AIN;
}


uint8_t convert_gain_bits(uint8_t gain) {
  // Converts the numerical gain to 3 gain select bits (p.25).
  // gain - one of 1, 8, 16, 32, 64, 128

  // Add breaks just in case
  switch (gain) {
    case 1:
      return 0x00;
      break;
    case 8:
      return 0x03;
      break;
    case 16:
      return 0x04;
      break;
    case 32:
      return 0x05;
      break;
    case 64:
      return 0x06;
      break;
    case 128:
      return 0x07;
      break;
    default:
      return 0x00;
      break;
  }
}

void calibrate_adc(uint8_t mode_select_bits) {
    // TODO - NOT TESTED YET

    // Read from mode register
    uint32_t mode_data = read_ADC_register(MODE_ADDR);

    // Clear and set mode select bits
    mode_data &= 0xff1fffff;
    mode_data |= ( ((uint32_t) mode_select_bits) << 21 );

    // Write to mode register
    write_ADC_register(MODE_ADDR, mode_data);

    // Check the state of PB0 on the 32M1, which is MISO

    // Wait for _RDY_ to go high (when the calibration starts)
    while (!bit_is_set(PINB, PB0)){
    continue;
    }
    // Wait for _RDY_ to go low (when the calibration finishes)
    while (bit_is_set(PINB, PB0)){
    continue;
    }
}

void enable_cont_conv_mode(void) {
  // Continuous conversion mode

  // Read from mode register
  uint32_t mode_data = read_ADC_register(MODE_ADDR);

  // Clear and set mode select bits
  mode_data &= 0xff1fffff;
  mode_data |= ( ((uint32_t) CONT_CONV) << 21 );

  // Write to mode register
  write_ADC_register(MODE_ADDR, mode_data);
}

/*void set_PGA(uint8_t gain) {
  // Sets the configuration register's bits for a specified programmable gain.
  // gain - one of 1, 8, 16, 32, 64, 128 (2 and 4 are reserved/unavailable, see p. 25)

  // Convert gain to 3 bits
  uint8_t gain_bits = convert_gain_bits(gain);

  uint8_t gain = 128;

  set_PGA(gain);

  // Write to configuration register
  write_ADC_register(CONFIG_ADDR, config_data);

  // Calibrate the ADC and re-enable continuous conversion mode
  calibrate_adc(SYS_ZERO_SCALE_CALIB);
  calibrate_adc(SYS_FULL_SCALE_CALIB);
  enable_cont_conv_mode();
}*/

void set_PGA(uint8_t gain) {
  // Sets the configuration register's bits for a specified programmable gain.
  // gain - one of 1, 8, 16, 32, 64, 128 (2 and 4 are reserved/unavailable, see p. 25)

  // Convert gain to 3 bits
  uint8_t gain_bits = convert_gain_bits(gain);

  // Read from configuration register
  uint32_t config_data = read_ADC_register(CONFIG_ADDR);

  // Clear gain bits and set
  config_data &= 0xfffffff8;
  config_data |= gain_bits;

  // Write to configuration register
  write_ADC_register(CONFIG_ADDR, config_data);
}
