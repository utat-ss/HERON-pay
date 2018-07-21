/*
Temperature, pressure, and humidity reading on the PAY-SSM.

AUTHORS: Russel Brown, Shimi Smith, Bruno Almeida, Dylan Vogel
*/

#include "env_sensors.h"

uint16_t pres_prom_data[8];


void temp_init(void) {
    init_cs(TEMP_CS_PIN, &TEMP_CS_DDR);
    set_cs_high(TEMP_CS_PIN, &TEMP_CS_PORT);

    // TODO - check this
//     pex_set_dir_b(TEMP_PEX_B_SHUTDOWN, PEX_DIR_OUTPUT); // Sets chip selects to low.
//     pex_set_gpio_b_high(TEMP_PEX_B_SHUTDOWN);  // Writes them high to disable CS
//     pex_set_gpio_b_low(TEMP_PEX_B_SHUTDOWN); // Clears the shutdown bit.
}

uint16_t temp_read_raw_data(void){
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
    uint16_t raw_data = 0;

    set_cs_low(TEMP_CS_PIN, &TEMP_CS_PORT);  //write CS low
    raw_data |= send_spi(0x00);
    raw_data <<= 8;
    raw_data |= send_spi(0x00);
    /*send_spi(0x00);
    send_spi(0x00);*/
    set_cs_high(TEMP_CS_PIN, &TEMP_CS_PORT);

    return raw_data;
}




void hum_init(void) {
    init_cs(HUM_CS_PIN, &HUM_CS_DDR);
    set_cs_high(HUM_CS_PIN, &HUM_CS_PORT);
}

// The data is formatted as {0b00, humidity (14 bits), temperature (14 bits), 0b00}
uint32_t hum_read_raw_data(void) {
    uint32_t data = 0;

    set_cs_low(HUM_CS_PIN, &HUM_CS_PORT);
    data |= send_spi(0x00);
    data <<= 8;
    data |= send_spi(0x00);
    data <<= 8;
    data |= send_spi(0x00);
    data <<= 8;
    data |= send_spi(0x00);
    set_cs_high(HUM_CS_PIN, &HUM_CS_PORT);

    return data;
}

uint16_t hum_read_raw_humidity(void) {
    uint32_t raw_data = hum_read_raw_data();
    return ((uint16_t)(raw_data >> 16)) & 0x3FFF;
}

uint16_t hum_read_raw_temperature(void) {
    uint32_t raw_data = hum_read_raw_data();
    return ((uint16_t)(raw_data >> 2)) & 0x3FFF;
}

double hum_convert_raw_temperature_to_temperature(uint16_t raw_temperature){
    return ((double) raw_temperature) / ((1 << 14) - 2.0) * 165.0 - 40.0;
}




void pres_init(void) {
    init_cs(PRES_CS_PIN, &PRES_CS_DDR);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    pres_reset();

    // read the calibration PROM
    for(uint8_t i = 0; i < 8; i++){
        pres_prom_data[i] = pres_read_prom(i);
    }
}

void pres_reset(void){
    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(PRES_RESET_SPI);
    _delay_ms(3);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);
}

// reads the calibration coefficients from the PROM
uint16_t pres_read_prom(uint8_t address){
    uint16_t data = 0;

    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi((address << 1) | PRES_PROM_BASE);
    data |= ((uint16_t) send_spi(0x00)) << 8;
    data |= ((uint16_t) send_spi(0x00));
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    return data;
}

// reads the uncompensated pressure or temperature depending on the command given
uint32_t pres_read_raw_uncompensated_data(uint8_t cmd){
    uint32_t data = 0;

    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(cmd);
    _delay_ms(10);  // wait for adc conversion to complete  -- note that this doesn't take as long for smaller resolutions
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(PRES_ADC_READ);
    data |= ((uint32_t) send_spi(0x00)) << 16;
    data |= ((uint32_t) send_spi(0x00)) << 8;
    data |= ((uint32_t) send_spi(0x00));
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    return data;
}

uint32_t pres_read_raw_uncompensated_pressure(void){
    return pres_read_raw_uncompensated_data(PRES_D1_4096);  // pressure
}

uint32_t pres_read_raw_uncompensated_temperature(void){
    return pres_read_raw_uncompensated_data(PRES_D2_4096);  // temperature
}

// Returns - D1 is pressure --- D2 is temperature
// Returns - raw pressure - need to treat as signed int32_t and divide by 1000 to get the value in kPa
// raw_temperature - return value (value of TEMP)
// gives 24 bit result
// from p.13 of datasheet
uint32_t pres_convert_raw_uncompensated_data_to_raw_pressure(uint32_t D1, uint32_t D2, uint32_t *raw_temperature){
    int32_t dT = (int32_t)D2 - ((int32_t)pres_prom_data[5] * 256);  // difference between actual and reference temperature
    int32_t TEMP = 2000 + ((int64_t)dT * (int64_t)pres_prom_data[6]) / 8388608LL;  // actual temperature

    int64_t OFF = ((int64_t)pres_prom_data[2] * 65536) + (pres_prom_data[4] * (int64_t)dT / 128);  // offset at actual temperature
    int64_t SENS = ((int64_t)pres_prom_data[1] * 32768) + (pres_prom_data[3] * (int64_t)dT) / 256;  // sensetivity at actual temperature

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
    if (raw_temperature != NULL) {
        *raw_temperature = (uint32_t) TEMP;
    }

    return (uint32_t) pressure;
}

uint32_t pres_read_raw_pressure(void) {
    uint32_t D1 = pres_read_raw_uncompensated_pressure();
    uint32_t D2 = pres_read_raw_uncompensated_temperature();
    return pres_convert_raw_uncompensated_data_to_raw_pressure(D1, D2, NULL);
}

// Converts the temperature result of `pres_convert_raw_uncompensated_data_to_raw_pressure` to the actual value
// Returns the value in ˚C
float pres_convert_raw_temperature_to_temperature(uint32_t raw_temperature) {
    // TODO - check conversion
    int32_t signed_raw_temperature = (int32_t) raw_temperature;
    return ((float) signed_raw_temperature) / 100.0;
}
