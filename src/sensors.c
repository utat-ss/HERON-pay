/*
Temperature, pressure, and humidity reading on the PAY-SSM.

AUTHORS: Russel Brown, Shimi Smith, Bruno Almeida, Dylan Vogel
*/

#include "sensors.h"

void sensor_setup(){
  uint8_t i;

  init_port_expander();

  for (i = 1; i < 5; i++) { // NEEDS TO BE CHANGED WHEN PRES SOLDERED ON
    set_dir_b(SSM, i, 0); // Sets chip selects to low.
    set_gpio_b(SSM, i);  // Writes them high to disable CS
  }
  clear_gpio_b(SSM, TEMP_SHUTDOWN); // Clears the shutdown bit.
  //set_dir_b(SSM, TEMP_CS, 0);
  //set_gpio_b(SSM, TEMP_CS);
  init_pressure_sensor();
}

float read_temperature(){
  return convert_temperature(read_raw_temperature());
}

uint16_t read_raw_temperature(){
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

  return *(uint16_t *)(&temp_sensor_data);  // type punning - TODO double check this works as expected on 32m1
}

float convert_temperature(uint16_t raw_temp_data){
  int16_t signed_temp_data = *(int16_t *)(&raw_temp_data);
  return (((float)signed_temp_data) * 0.03125);
}

uint32_t read_raw_humidity() {
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
  return convert_humidity(read_raw_humidity());
}

double convert_humidity(uint32_t raw_humidity) {
  uint16_t humidity_bytes = (uint16_t)(raw_humidity >> 16) & ~(3 << 14);
  double humidity = (double)humidity_bytes / (pow(2, 14) - 2.0) * 100.0;

  return humidity;
}

double read_humidity_temp() {
  return convert_humidity_temp(read_raw_humidity_temp());
}

uint16_t read_raw_humidity_temp(){
  uint32_t data = read_raw_humidity();
  return (uint16_t)data >> 2;
}

double convert_humidity_temp(uint16_t raw_humidity_temp){
  double temperature = (double)raw_humidity_temp / (pow(2, 14) - 2.0) * 165.0 - 40.0;

  return temperature;
}

void init_pressure_sensor(){
  pressure_sensor_reset();

  // read the calibration PROM
  for(uint8_t i = 0; i < 8; i++){
    PROM_data[i] = read_PROM(i);
  }

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

uint32_t read_raw_pressure(){
  return pressure_sensor_read(D1_4096);  // pressure
}

uint32_t read_raw_pressure_temp(){
  return pressure_sensor_read(D2_4096);  // temperature
}

// D1 is pressure  ---  D2 is temperature
float convert_pressure(uint16_t *PROM_data, uint32_t D1, uint32_t D2){

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

//Returns pressure as an integer, if divided by 1000 the value is given in Pa

int32_t convert_pressure_to24bits(uint16_t *PROM_data, uint32_t D1, uint32_t D2){

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
  //float press = ((float)(pressure))/1000.0;  // in kPa
  //float temp = (float)(TEMP) / 100.0;  // in ˚C

  return pressure;
}
