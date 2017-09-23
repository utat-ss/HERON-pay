/*

	FILENAME: 			sensors.c
	DEPENDENCIES:		spi, pex

	DESCRIPTION:		Temperature, pressure, and humidity reading on the PAY-SSM.
	AUTHORS:				Russel Brown, Shimi Smith, Bruno Almeida, Dylan Vogel
	DATE MODIFIED:	2017-09-19
	NOTES:          Currently compatible with PAY-SSM v2 designed for FlatSat 1

                  If the PCB is updated, the location of TEMP_CS in temperature.h
                  may need to be changed.

  REVISION HISTORY:

    2017-09-19:   Created, added sensor code for temp, humidity, and pressure.

*/

#include "sensors.h"

void sensor_setup(){
  uint8_t i;

  port_expander_init();

  for (i = 1; i < 5; i++) { // NEEDS TO BE CHANGED WHEN PRES SOLDERED ON
    set_dir_b(SSM, i, 0); // Sets chip selects to low.
    set_gpio_b(SSM, i);  // Writes them high to disable CS
  }
  clear_gpio_b(SSM, TEMP_SHUTDOWN); // Clears the shutdown bit.
  //set_dir_b(SSM, TEMP_CS, 0);
  //set_gpio_b(SSM, TEMP_CS);
  //pressure_sensor_reset();
}

float read_temperature(){
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

  _delay_ms(130);
  int16_t temp_sensor_data;
	clear_gpio_b(SSM, TEMP_CS);  // SET CS LOW
	temp_sensor_data = send_spi(0x00);
	temp_sensor_data <<=  8;
	temp_sensor_data |= send_spi(0x00);
	temp_sensor_data >>= 2;
	/*send_spi(0x00);
	send_spi(0x00);*/
	set_gpio_b(SSM, TEMP_CS);

	return (((float)temp_sensor_data) * 0.03125);
}

uint32_t get_raw_humidity() {
  uint8_t i;
  uint32_t data = 0;
  // TODO WARNING - check if the PEX usage is right
  // The humidity sensor CS is connected to GPB3 on the PEX
  // set_cs_low(CS, &CS_PORT);
  clear_gpio_b(SSM, HUMID_CS);
  data |= send_spi(0x00);
  data <<= 8;
  data |= send_spi(0x00);
  data <<= 8;
  data |= send_spi(0x00);
  data <<= 8;
  data |= send_spi(0x00);
  // for(i = 0; i < 4; i++){
  //   data <<= 8;
  //   data |= send_spi(0x00);
  // }
  set_gpio_b(SSM, HUMID_CS);

  return data;
}

double read_humidity() {
  uint32_t data = get_raw_humidity();
  uint16_t humidity_bytes = (uint16_t)(data >> 16) & ~(3 << 14); // if you don't mask out the first two bits, you get interesting oscillating ones in front of the humidity output.

  //uint16_t temperature_bytes = (uint16_t)data >> 2;
  double humidity = (double)humidity_bytes / (pow(2, 14) - 2.0) * 100.0;
  //double temperature = (double)temperature_bytes / (pow(2, 14) - 2.0) * 165.0 - 40.0;

  return humidity;
}

double read_humidity_temp() {
  uint32_t data = get_raw_humidity();
  //uint16_t humidity_bytes = (uint16_t)(data >> 16) & ~(3 << 14);
  uint16_t temperature_bytes = (uint16_t)data >> 2;
  //double humidity = (double)humidity_bytes / (pow(2, 14) - 2.0) * 100.0;
  double temperature = (double)temperature_bytes / (pow(2, 14) - 2.0) * 165.0 - 40.0;

  return temperature;
}

void pressure_sensor_reset(){
  clear_gpio_b(SSM, CS_1);
  send_spi(RESET);
  _delay_ms(3);
  set_gpio_b(SSM, CS_1);
}

uint16_t read_PROM(uint8_t address){  // reads the callibration coefficients from the PROM
  uint16_t data;

  clear_gpio_b(SSM, CS_1);
	send_spi((address << 1) | PROM_BASE);
  data = (uint16_t)send_spi(0x00) << 8;
	data |= (uint16_t)send_spi(0x00);
	set_gpio_b(SSM, CS_1);

	return data;
}

uint16_t *get_PROM(uint16_t *PROM_data){
  uint8_t i;
  for(i = 0; i < 8; i++){
    *(PROM_data + i) = read_PROM(i);
  }
}

uint32_t pressure_sensor_read(uint8_t cmd){  // reads the uncompensated pressure or temperature depending on the command given
  uint32_t data;

	clear_gpio_b(SSM, CS_1);
	send_spi(cmd);
	_delay_ms(10);  // wait for adc conversion to complete  -- note that this doesn't take as long for smaller resolutions
	set_gpio_b(SSM, CS_1);

	clear_gpio_b(SSM, CS_1);
	send_spi(ADC_READ);
	data = (uint32_t)send_spi(0x00) << 16;
	data |= (uint32_t)send_spi(0x00) << 8;
	data |= (uint32_t)send_spi(0x00);
	set_gpio_b(SSM, CS_1);

	return data;
}

float read_pressure(uint16_t *PROM_data){

  // The data is only 24 bits
  uint32_t D1 = pressure_sensor_read(D1_4096);  // pressure
  uint32_t D2 = pressure_sensor_read(D2_4096);  // temperature

  int32_t dT = (int32_t)D2 - ((int32_t)PROM_data[5] * 256);  // difference between actual and reference temperature
  int32_t TEMP = 2000 + ((int64_t)dT * PROM_data[6]) / 8388608LL;  // actual temperature
  TEMP = (int32_t)TEMP;

  int64_t OFF = ((int64_t)PROM_data[2] * 65536) + (PROM_data[4] * (int64_t)dT / 128);  // offset at actual temperature
  int64_t SENS = ((int64_t)PROM_data[1] * 32768) + (PROM_data[3] * (int64_t)dT) / 256;  // sensetivity at actual temperature

  int64_t T2;
  int64_t OFF2;
  int64_t SENS2;

  if(TEMP < 2000){  // low temperature
    T2 = ((int64_t)dT * dT) / 2147483648ULL;
    T2 = (int32_t)T2;
    OFF2 = 3 * ((TEMP - 2000) * (TEMP - 2000));
    SENS2 = 7 * ((TEMP - 2000) * (TEMP - 2000)) / 8;

    if(TEMP < -1500){  // very low temperature
      SENS2 += 2 * ((TEMP + 1500) * (TEMP + 1500));
    }
  }
  else{  // high temperature
    T2 = 0;
    OFF2 = 0;
    SENS2 = 0;

    if(TEMP >= 4500){  // very high temperature
      SENS2 -= ((TEMP - 4500) * (TEMP - 4500)) / 8;
    }
  }

  TEMP -= T2;
  OFF -= OFF2;
  SENS -= SENS2;

  int32_t pressure = (D1 * (SENS / 2097152) - OFF) / 32768;
  float press = ((float)(pressure))/1000.0;  // in kPa

  float temp = (float)(TEMP) / 100.0;  // in ˚C

  return press;
}
