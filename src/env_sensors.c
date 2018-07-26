/*
Environmental sensors (temperature, pressure, humidity) on the PAY-SSM.

Temperature sensor - LM95071
http://www.ti.com/lit/ds/symlink/lm95071.pdf

Humidity sensor - HIH7131
https://sensing.honeywell.com/honeywell-sensing-humidicon-hih7000-series-product-sheet-009074-6-en.pdf
TODO - might want to use temperature data?

Pressure sensor - MS5803-05BA
http://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5803-05BA%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS5803-05BA_B3.pdf%7FCAT-BLPS0011
TODO - might want to use temperature data?

AUTHORS: Russel Brown, Shimi Smith, Bruno Almeida, Dylan Vogel
*/

#include "env_sensors.h"

uint16_t pres_prom_data[8];




void temp_init(void) {
    init_cs(TEMP_CS_PIN, &TEMP_CS_DDR);
    set_cs_high(TEMP_CS_PIN, &TEMP_CS_PORT);

    // TODO
    // // Enable continuous conversion mode
    // set_cs_low(TEMP_CS_PIN, &TEMP_CS_PORT);
    // send_spi(0x00);
    // send_spi(0x00);
    // send_spi(0x00);
    // send_spi(0x00);
    // set_cs_high(TEMP_CS_PIN, &TEMP_CS_PORT);

    // TODO - look into continuous conversion and shutdown (p.8)
}

// Read 16 bits of raw temperature data
// (INCLUDING the 0b11 on the right that is always there) from the temperature sensor.
uint16_t temp_read_raw_data(void) {
    // TODO - account for temperature conversion time (p.3)?

    // TODO - check why this delay works
    _delay_ms(130);

    uint16_t raw_data = 0;

    set_cs_low(TEMP_CS_PIN, &TEMP_CS_PORT);  //write CS low
    raw_data |= send_spi(0x00);
    raw_data <<= 8;
    raw_data |= send_spi(0x00);
    set_cs_high(TEMP_CS_PIN, &TEMP_CS_PORT);

    return raw_data;
}




void hum_init(void) {
    init_cs(HUM_CS_PIN, &HUM_CS_DDR);
    set_cs_high(HUM_CS_PIN, &HUM_CS_PORT);
}

// The data received over SPI is formatted as
// {0b00, humidity (14 bits), temperature (14 bits), 0b00}
// Returns 14 bits of raw data
uint16_t hum_read_raw_data(void) {
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




void pres_init(void) {
    init_cs(PRES_CS_PIN, &PRES_CS_DDR);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    pres_reset();

    // read the calibration PROM
    print("Pressure sensor PROM:\n");
    for (uint8_t i = 0; i < 8; i++){
        pres_prom_data[i] = pres_read_prom(i);
        print("%04X ", pres_prom_data[i]);
    }
    print("\n");
}

void pres_reset(void) {
    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(PRES_CMD_RESET);
    // TODO - check delay
    _delay_ms(3);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);
}

// reads the calibration coefficients from the PROM
// p.8,11
uint16_t pres_read_prom(uint8_t address) {
    uint16_t data = 0;

    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi((address << 1) | PRES_CMD_PROM_READ_BASE);
    data |= (uint16_t) send_spi(0x00);
    data <<= 8;
    data |= (uint16_t) send_spi(0x00);
    set_cs_high(PRES_CS_PIN, &PRES_CS_PORT);

    return data;
}

// Reads the raw uncompensated pressure or temperature depending on the command given
uint32_t pres_read_raw_uncomp_data(uint8_t cmd) {
    set_cs_low(PRES_CS_PIN, &PRES_CS_PORT);
    send_spi(cmd);
    // wait for adc conversion to complete
    // -- note that this doesn't take as long for smaller resolutions
    // TODO - check delay
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


// Reads 24 bits of raw data (pressure) - 0-6000 mbar with 0.03mbar resolution
// Gives 24 bit result
// From p.13 of datasheet
// 1 bar = 100,000 Pa
// 1 mbar = 100 Pa
uint32_t pres_read_raw_data(void) {
    // Use `int64_t` for everything to be safe
    int64_t C1 = (int64_t) pres_prom_data[1];
    int64_t C2 = (int64_t) pres_prom_data[2];
    int64_t C3 = (int64_t) pres_prom_data[3];
    int64_t C4 = (int64_t) pres_prom_data[4];
    int64_t C5 = (int64_t) pres_prom_data[5];
    int64_t C6 = (int64_t) pres_prom_data[6];

    // Digital pressure
    int64_t D1 = (int64_t) pres_read_raw_uncomp_data(PRES_CMD_D1_4096);
    // Digital temperature
    int64_t D2 = (int64_t) pres_read_raw_uncomp_data(PRES_CMD_D2_4096);

    // Difference between actual and reference temperature
    int64_t dT = D2 - (C5 * (1 << 8));
    // Actual temperature
    // TODO - check UL vs L vs LL
    int64_t TEMP = 2000 + (dT * C6 / (1LL << 23));

    // Offset at actual temperature
    int64_t OFF = (C2 * (1LL << 18)) + (C4 * dT / (1 << 5));
    // Sensitivity at actual temperature
    int64_t SENS = (C1 * (1LL << 17)) + (C3 * dT / (1 << 7));
    // Temperature compensated pressure
    int64_t P = ((D1 * SENS / (1LL << 21)) - OFF) / (1 << 15);
    print("P before second order: %ld\n", P);

    // Second order temperature compensation (p.14)
    // TODO - should this be used? should it go before the other calculation?
    // TODO - what are those signs on the diagram?
    int64_t T2;
    int64_t OFF2;
    int64_t SENS2;

    // Temp < 20C
    if (TEMP < 2000) {
        T2 = 3 * (dT * dT) / (1LL << 33);
        OFF2 = 3 * ((TEMP - 2000) * (TEMP - 2000)) / (1 << 3);
        SENS2 = 7 * ((TEMP - 2000) * (TEMP - 2000)) / (1 << 3);

        // Temp < -15C ?
        if (TEMP < -1500){
            SENS2 = SENS2 + (3 * (TEMP + 1500) * (TEMP + 1500));
        }
        // No else condition
    }

    // Temp >= 20C
    else {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }

    TEMP = TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;

    P = ((D1 * SENS / (1LL << 21)) - OFF) / (1 << 15);
    print("P after second order: %ld\n", P);

    return (uint32_t) ((int32_t) P);
}
