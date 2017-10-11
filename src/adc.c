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
      * bit[3] set to 1 - unipolar or bipolar
      * bit[2:0] set to 000 - programmable gain

      0b0000 0100 0000 0000 0000 1000
      0x040008;



*/

// We might want to trigger an interrupt on RDY


uint8_t select_channel(uint8_t channel_num){
  // Only need to select 5-7
  return (channel num - 1);
};

uint24_t read_channel(uint8_t channel_num){
  // Select channel from 5-7
  // Send appropriate byte to comm register
  // Delay until RDY bit low
  // Return 24 bit data
}

void set_PGA(uint8_t gain){
  // Take 'number number' for gain
  // eg 128
  // Convert into 3 bits and set config register
  // Read current config status
  // Mask read value
  // Send
}
