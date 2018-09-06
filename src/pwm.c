#include "pwm.h"

//frequency = 8/(prescaler * top * 2) [Mhz]
// 0 <= prescaler <= 4 (1, 8, 64, 256, 1024)
// note that if top == 0, then use 0.5 as the value for top in the formula
void OC0A(uint8_t prescaler, uint8_t top)
{
  uint8_t read_TCCR0A = TCCR0A;

  uint8_t write_TCCR0A = (read_TCCR0A & 0x3C) | (0x43);

  TCCR0A = write_TCCR0A;

  uint8_t read_TCCR0B = TCCR0B;
  
}
