/*
Environmental sensors (pressure, humidity) on the PAY-SSM v4.

Humidity sensor - HIH7131
https://sensing.honeywell.com/honeywell-sensing-humidicon-hih7000-series-product-sheet-009074-6-en.pdf

Pressure sensor - MS5803-05BA
http://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5803-05BA%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS5803-05BA_B3.pdf%7FCAT-BLPS0011

AUTHORS: Russel Brown, Shimi Smith, Bruno Almeida, Dylan Vogel
*/

#include "env_sensors.h"

/*
Initializes the humidity sensor.
*/
void init_hum(void) {
    init_cs(HUM_CS_PIN, &HUM_CS_DDR);
    set_cs_high(HUM_CS_PIN, &HUM_CS_PORT);
}

/*
Reads 14 bits of raw data from the humidity sensor.

The data received over SPI is formatted as
{0b00, humidity (14 bits), temperature (14 bits), 0b00}
*/
uint16_t read_hum_raw_data(void) {
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

    // Isolate the 14 bits for humidity
    return (data >> 16) & 0x3FFF;
}




uint16_t pres_prom_data[8];

/*
Initializes the pressure sensor.
*/
void init_pres(void) {
    init_cs(PRES_CS_PIN, &PRES_CS_DDR);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    reset_pres();

    // read the calibration PROM
    for (uint8_t i = 0; i < 8; i++){
        pres_prom_data[i] = read_pres_prom(i);
    }
}

/*
Resets the pressure sensor.
*/
void reset_pres(void) {
    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(PRES_CMD_RESET);
    // 2.8ms reload time (p.10)
    _delay_ms(3);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);
}

/*
Reads the calibration coefficients from the pressure sensor's PROM.
p.8,11
*/
uint16_t read_pres_prom(uint8_t address) {
    uint16_t data = 0;

    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi((address << 1) | PRES_CMD_PROM_READ_BASE);
    data |= (uint16_t) send_spi(0x00);
    data <<= 8;
    data |= (uint16_t) send_spi(0x00);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    return data;
}

/*
Reads 24 bits of raw uncompensated pressure or temperature data from the
pressure sensor's ADC, depending on the command given.
*/
uint32_t read_pres_raw_uncomp_data(uint8_t cmd) {
    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(cmd);
    // wait for adc conversion to complete
    // -- note that this doesn't take as long for smaller resolutions
    // 8.22ms ADC conversion (p.11)
    _delay_ms(10);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    uint32_t data = 0;
    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(PRES_CMD_ADC_READ);
    data |= (uint32_t) send_spi(0x00);
    data <<= 8;
    data |= (uint32_t) send_spi(0x00);
    data <<= 8;
    data |= (uint32_t) send_spi(0x00);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    return data;
}

/*
Converts pressure sensor register data (calibration data, digital pressure/temperature)
to raw data (24 bit result)
Conversion formulas from p.13

Debugging:
Can't use print() with 64-bit numbers, so need to represent it as a 32-bit number
(e.g. divide 64-bit number by 1,000,000 and print it as 32-bit, i.e. %ld or %lu)

TODO - test second order temperature compensation (< 20 C)
*/
uint32_t pres_reg_data_to_raw_data(
    uint16_t C1, uint16_t C2, uint16_t C3, uint16_t C4, uint16_t C5, uint16_t C6,
    uint32_t D1, uint32_t D2) {

    // Difference between actual and reference temperature
    int32_t dT_1 = C5 * (1L << 8);
    int32_t dT = D2 - dT_1;

    // Actual temperature
    int32_t TEMP_1 = (((uint64_t) dT) * C6 / (1LL << 23));
    int32_t TEMP = 2000 + TEMP_1;

    // Offset at actual temperature
    int64_t OFF_1 = C2 * (1LL << 18);
    int64_t OFF_2 = ((int64_t) C4) * dT / (1LL << 5);
    int64_t OFF = OFF_1 + OFF_2;

    // Sensitivity at actual temperature
    int64_t SENS_1 = C1 * (1LL << 17);
    int64_t SENS_2 = ((int64_t) C3) * dT / (1LL << 7);
    int64_t SENS = SENS_1 + SENS_2;

    // Temperature compensated pressure
    int64_t P_1 = D1 * SENS / (1LL << 21);
    int32_t P = (P_1 - OFF) / (1LL << 15);

    // Second order temperature compensation (p.14)
    // TODO - should this be used? should it go before the other calculation?
    // TODO - what are those signs on the datasheet diagram?
    int32_t T2;
    int64_t OFF2;
    int64_t SENS2;

    // Temp < 20 C
    if (TEMP < 2000) {
        int64_t T2_1 = ((int64_t) dT) * dT;
        T2 = 3 * T2_1 / (1LL << 33);

        int64_t OFF2_1 = ((int64_t) (TEMP - 2000)) * (TEMP - 2000);
        OFF2 = 3 * OFF2_1 / (1LL << 3);

        int64_t SENS2_1 = ((int64_t) (TEMP - 2000)) * (TEMP - 2000);
        SENS2 = 7 * SENS2_1 / (1LL << 3);

        // Temp < -15C ?
        if (TEMP < -1500){
            int64_t SENS2_2 = 3 * ((int64_t) (TEMP + 1500)) * (TEMP + 1500);
            SENS2 = SENS2 + SENS2_2;
        }
        // No else condition
    }

    // Temp >= 20 C
    else {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }

    TEMP = TEMP - T2;

    OFF = OFF - OFF2;

    SENS = SENS - SENS2;

    P_1 = D1 * SENS / (1LL << 21);
    P = (P_1 - OFF) / (1LL << 15);

    return (uint32_t) P;
}

/*
Reads 24 bits of raw data from the pressure sensor - 0-6000 mbar with 0.01mbar resolution per bit
Datasheet says 0.03mbar resolution, but should be 0.01mbar
*/
uint32_t read_pres_raw_data(void) {
    // Calibration data
    uint16_t C1 = pres_prom_data[1];
    uint16_t C2 = pres_prom_data[2];
    uint16_t C3 = pres_prom_data[3];
    uint16_t C4 = pres_prom_data[4];
    uint16_t C5 = pres_prom_data[5];
    uint16_t C6 = pres_prom_data[6];

    // Digital pressure
    uint32_t D1 = read_pres_raw_uncomp_data(PRES_CMD_D1_4096);
    // Digital temperature
    uint32_t D2 = read_pres_raw_uncomp_data(PRES_CMD_D2_4096);

    return pres_reg_data_to_raw_data(C1, C2, C3, C4, C5, C6, D1, D2);
}
