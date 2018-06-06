/*
Port expander functions.

AUTHORS: Dylan Vogel, Shimi Smith

TODO: Change the macro name for the PEX CS in case another program wants to use it
*/

#include "pex.h"

// Resets the SSM port expander
void reset_ssm_pex(){
    set_cs_low(SSM_RST, &RST_PORT);
    _delay_ms(1);            // minimum 1 microsecond
    set_cs_high(RST, &RST_PORT);
    _delay_ms(1);
}

// Resets the sensor port expanders
void reset_pex(){
    set_cs_low(RST, &RST_PORT);
    _delay_ms(1);        // minimum 1 microsecond
    set_cs_high(RST, &RST_PORT);
    _delay_ms(1);
}

// Initializes port expander resets, chip selects, and SPI
void init_port_expander(){
    init_cs(RST, &RST_DDR);
    init_cs(SSM_RST, &SSM_RST_DDR);
    set_cs_high(RST, &RST_PORT);
    set_cs_high(SSM_RST, &SSM_RST_PORT);

    init_cs(CS, &CS_DDR);
    set_cs_high(CS, &CS_PORT);
    init_spi();
    port_expander_write(0x00, IOCON, IOCON_DEFAULT);
}

// Sets pin 'pin' on GPIOA high on port expander 'address'
void set_gpio_a(uint8_t address, uint8_t pin){
    uint8_t register_state = port_expander_read(address, GPIO_BASE);
    port_expander_write(address, GPIO_BASE, register_state | (1 << pin));
}

// Sets pin 'pin' on GPIOB high on port expander 'address'
void set_gpio_b(uint8_t address, uint8_t pin){
    uint8_t register_state = port_expander_read(address, GPIO_BASE + 0x01);
    port_expander_write(address, GPIO_BASE + 0x01, register_state | (1 << pin));
}

// Sets pin 'pin' on GPIOA low on port expander 'address'
void clear_gpio_a(uint8_t address, uint8_t pin){
    uint8_t register_state = port_expander_read(address, GPIO_BASE);
    port_expander_write(address, GPIO_BASE, register_state & ~(1 << pin));
}

// Sets pin 'pin' on GPIOB low on port expander 'address'
void clear_gpio_b(uint8_t address, uint8_t pin){
    uint8_t register_state = port_expander_read(address, GPIO_BASE + 0x01);
    port_expander_write(address, GPIO_BASE + 0x01, register_state & ~(1 << pin));
}

// Sets the direction of pin 'pin' on GPIOA on port expander 'address' to state 'state'
// state == 0 corresponds to output, state == 1 corresponds to input
void set_dir_a(uint8_t address, uint8_t pin, uint8_t state){
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

// Sets the direction of pin 'pin' on GPIOA on port expander 'address' to state 'state'
// state == 0 corresponds to output, state == 1 corresponds to input
void set_dir_b(uint8_t address, uint8_t pin, uint8_t state){
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

// Writes data 'data' to register 'register_addr' on port expander 'address'
void port_expander_write(uint8_t address, uint8_t register_addr, uint8_t data){
    set_cs_low(CS, &CS_PORT);  //write CS low
    send_spi(write_control_byte | address << 1);
    send_spi(register_addr);
    send_spi(data);
    set_cs_high(CS, &CS_PORT);  //write CS high
    _delay_ms(10);
}

// Reads data on register 'register_addr' on port expander 'address'
uint8_t port_expander_read(uint8_t address, uint8_t register_addr){
    uint8_t read_data;
    set_cs_low(CS, &CS_PORT);
    send_spi(read_control_byte | address << 1);
    send_spi(register_addr);
    read_data = send_spi(0x00);
    set_cs_high(CS, &CS_PORT);
    _delay_ms(10);

    return read_data;
}
