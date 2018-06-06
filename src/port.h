#include <stdint.h>
#include <avr/io.h>

#define INPUT 0
#define OUTPUT 1
#define HIGH 1
#define LOW 0

typedef volatile uint8_t* Port;

void setPinMode(uint8_t pin, Port DDR, uint8_t mode);
void write(uint8_t pin, Port PORT, uint8_t value);
void toggle(uint8_t pin, Port PORT);
uint8_t read(uint8_t pin, uint8_t PIN);
