#include "pwm.h"

void stop_timer()
{
  GTCCR |= 0x81;
}

void start_timer()
{
  GTCCR &= 0x7F;
}

void init_pwm_8bit(uint8_t prescaler, uint8_t top)
{
  /*
  Initializes and configures 8-bit timer for fast (single-slope) PWM output
  with variable frequence and fixed 50% duty cycle

  Prescaler: Divide the system clock (8 MHz) by one of five pre-set constants,
  (1, 8, 64, 256, 1024), 0 <= prescaler <= 4

  Top: Maximum value to which the counter will go before inverting the waveform,
  and resetting to 0x00, 0 <= top <= 255

  Frequency = 8/(prescaler * (top + 1) * 2) [Mhz]

  Duty-cyle: Invariably 50%, cannot be changed for this timer
  in the current configuration

  */

  uint8_t read_TCCR0B = TCCR0B;
  uint8_t write_TCCR0B = ((read_TCCR0B & 0x30) | (0x09)) + prescaler;

  /* TCCR0B7:6 Force output compare (write to zero to deactivate)
  TCCR0B5:4 Reserved by manufacturer (copy, and preserve)
  TCCR0B3 WGM02, see below with other WGM0 bits
  TCCR0B2:0 Clock select bits; because they are the lowest bits in the byte,
  the prescaler can be added to the byte to shift to the correct prescaler
  */

  TCCR0B = write_TCCR0B;

  uint8_t read_TCCR0A = TCCR0A;
  uint8_t write_TCCR0A = (read_TCCR0A & 0x3C) | (0x43);

  /* TCCR0A7:6 Set to 0x2, combined with WGM02 = 1, toggle OC0A (output)
  on compare match (with OCR0A)
  TCCR0A5:4 Equivalent bits for 0C0B, reserved for future hardware
  configurations
  TCCR0A3:2 Reserved by manufacturer (copy, and preserve)
  TCCR0B3 and TCCR0A1:0 Termed WGM03:0, determine waveform output, set to 0x7
  for Fast PWM with OCR0A as TOP
  */

  TCCR0A = write_TCCR0A;

  OCR0A = top; //set the top of the counter

  TIMSK0 &= 0xF8; //Disable interrupts

  DDRD |= _BV(0x03); //Change port to output in the data direction register

  PRR &= 0xF7;
  //Disable power reduction to the timer in the power reduction register
}

void init_pwm_16bit(uint8_t prescaler, uint16_t top, uint16_t duty_cycle)
{
  /*
  Initializes and configures 16-bit timer for fast (single-slope) PWM output
  with variable frequency and variable duty cycle

  Prescaler: Divide the system clock (8 MHz) by one of five pre-set constants,
  (1, 8, 64, 256, 1024), 0 <= prescaler <= 4

  Top: Maximum value to which the counter will go before inverting the waveform,
  and resetting to 0x00, 0 <= top <= 255

  Duty-cycle: Changes the percentage ON over a period, = duty_cycle/top,
  duty-cycle <= top (otherwise you will got an always-on signal)

  Frequency = 8/(prescaler * (top + 1)) [Mhz]

  */

  uint8_t read_TCCR1A = TCCR1A;
  uint8_t write_TCCR1A = (read_TCCR1A & 0xAA) | 0x22;

  /* TCCR1A7:6 For OC1A, reserved for future hardware configuration
  TCCR1A5:4 Set to 0x2, for OC1B (the output) to clear at compare match (with ICR1)
  and set at TOP (so higher ICR1 aka higher duty_cycle,
  increases the percentage ON)
  TCCR1A3:2 Reserved by manufacturer
  TCCR1A1:0 WGM1[1:0] see below for WGM1 bit descriptions
  */

  TCCR1A = write_TCCR1A;

  uint8_t read_TCCR1B = TCCR1B;
  uint8_t write_TCCR1B = ((read_TCCR1B & 0x40) | 0x99) + prescaler;

  /* TCCR1B7:6 Noise cancelling and edge selection for input capture
  functionality (unused); write to 0x2
  TCCR1B5 Write to 0 for future compatibility from manufacturer
  TCCR1B4:3 and TCCR1A1:0 WGM1[3:0], set to 0xE for fast PWM (single-slope)
  with ICR1 as the maximum value, and signal polarity switching at OCR1B
  TCCR1B2:0 Clock select bits; because they are the lowest bits in the byte,
  the prescaler can be added to the byte to shift to the correct prescaler
  */

  TCCR1B = write_TCCR1B;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  /* Writing to "16-bit" registers is a two-clock-cycle operation, atomic blocks
  are required to ensure that the operation cannot be interrupted */
  {
    ICR1 = top; //set the top of the counter
    OCR1B = duty_cycle; //set the duty cycle
  }

  TIMSK1 &= 0xD8; //disable interrupts

  PRR &= 0xEF; //disable power reduction to the timer

  DDRC |= _BV(0x01); //set the data direction port to output

}
