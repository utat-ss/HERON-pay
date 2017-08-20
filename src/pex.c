#include "pex.h"

void port_expander_init(){

	init_cs(CS, &CS_DDR);
	set_cs_high(CS, &CS_PORT);
	init_spi();

	// port_expander_write(IOCON, 0b00001000); 
	port_expander_write(IOCON, 0b00101000);  // try this maybe - turns sequential on

	port_expander_write(IODIR_BASE, GPIOA_config);
	// port_expander_write(IODIR_BASE + 0x01, GPIOB_config);

}

void port_expander_write(uint8_t register_addr, uint8_t data){

	set_cs_low(CS, &CS_PORT);  //write CS low
	send_spi(write_control_byte);
	send_spi(register_addr);
	send_spi(data);
	set_cs_high(CS, &CS_PORT);  //write CS high

}

char port_expander_read(uint8_t register_addr){

  char read_data;
  set_cs_low(CS, &CS_PORT);
  send_spi(read_control_byte);
  send_spi(register_addr);
  read_data = send_spi(0x00);
  set_cs_high(CS, &CS_PORT);

  return read_data;
}

void blinkLED_PEX(void){

	port_expander_init();

	while(1){

		port_expander_write(GPIO_BASE, 0b00000001);  //  Writes high to GP0
		_delay_ms(150);
		port_expander_write(GPIO_BASE, 0b00000000);
		_delay_ms(150);

	}
}

int main(){
	blinkLED_PEX();
}
