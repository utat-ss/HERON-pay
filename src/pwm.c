#include "pwm.h"

//frequency = 8/(prescaler * top * 2) [Mhz]
// 0 <= prescaler <= 4 (1, 8, 64, 256, 1024)
// note that if top == 0, then use 0.5 as the value for top in the formula
void OC0A(uint8_t prescaler, uint8_t top)
{
  uint8_t read_TCCR0B = TCCR0B;
  uint8_t write_TCCR0B = ((read_TCCR0B & 0x30) | (0x09)) + prescaler;
  TCCR0B = write_TCCR0B;

  uint8_t read_TCCR0A = TCCR0A;
  uint8_t write_TCCR0A = (read_TCCR0A & 0x3C) | (0x43);
  TCCR0A = write_TCCR0A;

  OCR0A = top;

  uint8_t read_TIMSK0 = TIMSK0;
  uint8_t write_TIMSK0 = (read_TIMSK0 & 0xF8) | 0x00;
  TIMSK0 = write_TIMSK0;

  DDRD |= _BV(0x03);
}

// void OC1B(uint8_t prescaler, uint16_t top)
// {
//
// }
