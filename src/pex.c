/*

	FILENAME: 			pex.c
	DEPENDENCIES:		spi

	DESCRIPTION:		Port expander functions.
	AUTHORS:				Dylan Vogel, Shimi Smith
	DATE MODIFIED:	2017-09-01
	NOTES:


*/

#include "pex.h"
#include "spi.h"

void port_expander_init(){
	init_cs(CS, &CS_DDR);
	set_cs_high(CS, &CS_PORT);
	init_spi();
	port_expander_write(0x00, IOCON, IOCON_DEFAULT);
}

void set_gpio_a(uint8_t address, uint8_t pin_states){
	port_expander_write(address, GPIO_BASE, pin_states);
}

void set_gpio_b(uint8_t address, uint8_t pin_states){
	port_expander_write(address, GPIO_BASE + 0x01, pin_states);
}

void port_expander_write(uint8_t address, uint8_t register_addr, uint8_t data){
	set_cs_low(CS, &CS_PORT);  //write CS low
	send_spi(write_control_byte | address<<1);
	send_spi(register_addr);
	send_spi(data);
	set_cs_high(CS, &CS_PORT);  //write CS high
}

uint8_t port_expander_read(uint8_t address, uint8_t register_addr){
  uint8_t read_data;
  set_cs_low(CS, &CS_PORT);
  send_spi(read_control_byte | address<<1);
  send_spi(register_addr);
  read_data = send_spi(0x00);
  set_cs_high(CS, &CS_PORT);

  return read_data;
}
