/*

  ADC DEVELOPMENT CODE
  Written by Bruno and Dylan

  ADC7194


  TODO:
    * Decide on a config register default state
      * bit[23] to 0 - chop bit
      * bit[20] set to 0 — ref select
      * bit[18] should be 1
      * bit[16] set to 0 — temp bit
      * bit[15:8] – channel select
      * bit[7] set to 0 – burnout
      * bit[6] set to 0 - ref detect
      * bit[4] set to 0 – buffer, might want to enable
      * bit[3] set to 1 - unipolar or bipolar (1 for unipolar)
      * bit[2:0] set to 000 - programmable gain

      0b0000 0100 0000 0000 0000 1000
      0x040008;



*/

/*
  Datasheet - Important Pages

  24 - configuration register - select gain
    - CON2-CON0 - gain select

  32 - overall communication, read/write operations to registers

  33 - single conversion mode

  34 - continuous conversion mode
*/



// We might want to trigger an interrupt on RDY (falling edge)

#include "adc.h"

void init_adc(){
  // We're assuming that port_expander_init() has already been called.
  int i;

  // Set all the directions needed for sensor board functionality.
  for (i = 0; i < 4; i++){
    set_dir_a(SENSOR_PCB, i, 0);  //Set all the LED outputs
    clear_gpio_a(SENSOR_PCB, i);
  }
  for (i = 0; i < 2; i++){
    set_dir_b(SENSOR_PCB, i, 0); // Set ITF and ADC CS
    set_gpio_b(SENSOR_PCB, i);
  }

  // TODO - is this supposed to say transfer_ADC()?
  // adc_transfer()

  // TODO - write the default configuration register state we want?
}

// TODO - what is this supposed to do?
uint8_t * transfer_ADC();

uint8_t select_channel(uint8_t channel_num){
  // Only need to select 5-7
  // return (channel num - 1);
};

uint32_t read_channel(uint8_t channel_num){
  // Select channel from 5-7
  // Send appropriate byte to comm register
  // Delay until RDY bit low
  // Return 24 bit data
}




/*
  Converts the numerical gain to 3 gain select bits (p.25).
  gain - one of 1, 8, 16, 32, 64, 128
*/
uint8_t convert_gain_bits(uint8_t gain) {
  // Add breaks just in case
  switch (gain) {
    case 1:
      return 0b000;
      break;
    case 8:
      return 0b011;
      break;
    case 16:
      return 0b100;
      break;
    case 32:
      return 0b101;
      break;
    case 64:
      return 0b110;
      break;
    case 128:
      return 0b111;
      break;
    default:
      return 0b000;
      break;
  }
}


/*
  Reads the current state of the configuration register.
*/
uint32_t read_config_register() {
  // Set CS low
  clear_gpio_b(SENSOR_PCB, ADC_CS);

  // Write the communication register byte
  // to read from the configuration register (p. 19)
  send_spi(COMM_BYTE_READ | (CONFIG_ADDR << 3));

  // Read (24 bits)
  uint32_t config_data = send_spi(0);
  config_data = config_data << 8;
  config_data = config_data | send_spi(0);
  config_data = config_data << 8;
  config_data = config_data | send_spi(0);

  // Set CS high
  set_gpio_b(SENSOR_PCB, ADC_CS);

  return config_data;
}


/*
  Writes a new state to the configuration register.
*/
void write_config_register(uint32_t config_data) {
  // Set CS low
  clear_gpio_b(SENSOR_PCB, ADC_CS);

  // Write the communication register byte
  // to write to the configuration register (p.19)
  send_spi(COMM_BYTE_WRITE | (CONFIG_ADDR << 3));

  // Write the new state of the configuration register (24 bits)
  send_spi((uint8_t)(config_data >> 16));
  send_spi((uint8_t)(config_data >> 8));
  send_spi((uint8_t)(config_data));

  // Set CS high
  set_gpio_b(SENSOR_PCB, ADC_CS);
}


/*
  Sets the communication register's bits for a specified programmable gain.
  gain - one of 1, 8, 16, 32, 64, 128 (2 and 4 are reserved/unavailable, p. 25)
*/
void set_PGA(uint8_t gain) {
  // Convert gain to 3 bits
  uint8_t gain_bits = convert_gain_bits(gain);

  // Read from configuration register
  uint32_t config_data = read_config_register();


  // Mask configuration bits to set the gain

  // Clear bits 2-0 (set to 0)
  for (int i = 2; i >= 0; i--) {
    config_data = config_data & ~(1 << i);
  }

  // Set bits 2-0 to the appropriate gain
  config_data = config_data | gain_bits;


  // Write to configuration register
  write_config_register(config_data);
}
