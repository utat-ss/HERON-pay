/*

	FILENAME: 			pex.c
	DEPENDENCIES:		spi

	DESCRIPTION:		Port expander functions.
	AUTHORS:				Dylan Vogel, Shimi Smith
	DATE MODIFIED:	2017-09-19
	NOTES:
									TODO:
									* Change the macro name for the PEX CS in case another program wants to use it

	REVISION HISTORY:


		2017-10-27: 	Added comments. Changed port_expander_init to init_port_expander
									to match convention elsewhere
	`	2017-09-22:		Fixed the switch-case statements in set_gpio_x`
		2017-09-19: 	Added "clear_gpio_x" functions to clear a GPIO register bit.
									"set_gpio_x" now sets a GPIO register bit.
									Changed the behaviour of "set_gpio_x" to work on a pin-by-pin basis,
									rather than through writing to the entire register.
		2017-09-01: 	Created initial set of functions.

*/

#include "pex.h"

void reset_ssm_pex(){
	// Reset the SSM port expander
	set_cs_low(SSM_RST, &RST_PORT);
	_delay_ms(1);			// minimum 1 microsecond
	set_cs_high(RST, &RST_PORT);
	_delay_ms(1);
}

void reset_pex(){
	// Reset the sensor port expanders
	set_cs_low(RST, &RST_PORT);
	_delay_ms(1);		// minimum 1 microsecond
	set_cs_high(RST, &RST_PORT);
	_delay_ms(1);
}

void init_port_expander(){
	// Initialize port expander resets, chip selects, and SPI
	init_cs(RST, &RST_DDR);
	init_cs(SSM_RST, &SSM_RST_DDR);
	set_cs_high(RST, &RST_PORT);
	set_cs_high(SSM_RST, &SSM_RST_PORT);

	init_cs(CS, &CS_DDR);
	set_cs_high(CS, &CS_PORT);
	init_spi();
	port_expander_write(0x00, IOCON, IOCON_DEFAULT);
}

void set_gpio_a(uint8_t address, uint8_t pin){
	// Set pin 'pin' on GPIOA high on port expander 'address'
	uint8_t register_state = port_expander_read(address, GPIO_BASE);
	port_expander_write(address, GPIO_BASE, register_state | (1 << pin));
}

void set_gpio_b(uint8_t address, uint8_t pin){
	// Set pin 'pin' on GPIOB high on port expander 'address'
	uint8_t register_state = port_expander_read(address, GPIO_BASE + 0x01);
	port_expander_write(address, GPIO_BASE + 0x01, register_state | (1 << pin));
}

void clear_gpio_a(uint8_t address, uint8_t pin){
	// Set pin 'pin' on GPIOA low on port expander 'address'
	uint8_t register_state = port_expander_read(address, GPIO_BASE);
	port_expander_write(address, GPIO_BASE, register_state & ~(1 << pin));
}

void clear_gpio_b(uint8_t address, uint8_t pin){
	// Set pin 'pin' on GPIOB low on port expander 'address'
	uint8_t register_state = port_expander_read(address, GPIO_BASE + 0x01);
	port_expander_write(address, GPIO_BASE + 0x01, register_state & ~(1 << pin));
}

void set_dir_a(uint8_t address, uint8_t pin, uint8_t state){
	// Set the direction of pin 'pin' on GPIOA on port expander 'address' to state 'state'
	// state == 0 corresponds to output, state == 1 corresponds to input
	uint8_t register_state = port_expander_read(address, IODIR_BASE);
	switch (state) {
		case 0: // OUTPUT
			port_expander_write(address, IODIR_BASE, (register_state & ~(1 << pin)));
			break;
		case 1: // INPUT
			port_expander_write(address, IODIR_BASE, (register_state | (1 << pin)));
			break;
	}
}

void set_dir_b(uint8_t address, uint8_t pin, uint8_t state){
	// Set the direction of pin 'pin' on GPIOA on port expander 'address' to state 'state'
	// state == 0 corresponds to output, state == 1 corresponds to input
	uint8_t register_state = port_expander_read(address, IODIR_BASE + 0x01);
	switch(state) {
		case 0: // OUTPUT
			port_expander_write(address, IODIR_BASE + 0x01, (register_state & ~(1 << pin)));
			break;
		case 1: // INPUT
			port_expander_write(address, IODIR_BASE + 0x01, (register_state | (1 << pin)));
			break;
	}
}

void port_expander_write(uint8_t address, uint8_t register_addr, uint8_t data){
	// Write data 'data' to register 'register_addr' on port expander 'address'
	set_cs_low(CS, &CS_PORT);  //write CS low
	send_spi(write_control_byte | address << 1);
	send_spi(register_addr);
	send_spi(data);
	set_cs_high(CS, &CS_PORT);  //write CS high
	_delay_ms(10);
}

uint8_t port_expander_read(uint8_t address, uint8_t register_addr){
	// Read data on register 'register_addr' on port expander 'address'
  uint8_t read_data;
  set_cs_low(CS, &CS_PORT);
  send_spi(read_control_byte | address << 1);
  send_spi(register_addr);
  read_data = send_spi(0x00);
  set_cs_high(CS, &CS_PORT);
	_delay_ms(10);

  return read_data;
}
