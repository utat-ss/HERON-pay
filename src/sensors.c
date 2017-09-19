/*

	FILENAME: 			temperature.c
	DEPENDENCIES:		spi, pex

	DESCRIPTION:		Temperature reading from the LM95071.
	AUTHORS:				Russel Brown, Dylan Vogel
	DATE MODIFIED:	2017-09-19
	NOTES:          Currently compatible with PAY-SSM v2 designed for FlatSat 1

                  If the PCB is updated, the location of TEMP_CS in temperature.h
                  may need to be changed.

*/

#include "temperature.h"


float temperature_read(){
  // Read a temperature from the temperature sensor.

	//_delay_ms(50);
	/*
	set_cs_low(CS, &CS_PORT);
	send_spi(0x00);
	send_spi(0x00);
	GPIO_PORT &= ~_BV(GPIO);
	send_spi(0x00);
	send_spi(0x00);
	set_cs_high(CS, &CS_PORT);
	*/
	//_delay_ms(130);
	set_gpio_b(0, TEMP_CS);  // SET CS LOW
	temp_sensor_data = send_spi(0x00);
	temp_sensor_data <<=  8;
	temp_sensor_data |= send_spi(0x00);
	temp_sensor_data >>= 2;
	/*send_spi(0x00);
	send_spi(0x00);*/
	clear_gpio_b(0, TEMP_CS);

	return (((float)temp_sensor_data) * 0.03125);
}
