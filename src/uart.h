#include <avr/io.h>
#include <string.h>
#include <stdio.h>

#define F_IO 1000000UL // 1 MHz
#define BAUD_RATE 9600UL
#define BIT_SAMPLES 4UL

#define UART_TX PD3
#define UART_RX PD4

void init_uart();
void send_uart(const unsigned char *);
