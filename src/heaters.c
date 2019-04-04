/*
Comparator functionality for heaters. Requires thermistors and heaters/ammeter
attached to payload board

Author: Brytni Richards, Bruno Almeida

For IC COMPARATOR LO PWR SGL V SOT 23-5L Part # TS391ILT
Digikey Link: https://www.digikey.ca/product-detail/en/stmicroelectronics/TS391ILT/497-2275-1-ND/599229
Datasheet: https://www.st.com/content/ccc/resource/technical/document/datasheet/de/4c/b3/3d/64/7d/48/8e/CD00001660.pdf/files/CD00001660.pdf/jcr:content/translations/en.CD00001660.pdf
*/

#include "heaters.h"

// init is covered by dac_init()

// Sets temperature setpoint of heaters 1-4 (all connected to DAC A)
// raw_data - 12 bit DAC raw data for setpoint
void set_heaters_1_to_4_raw_setpoint(uint16_t raw_data) {
    set_dac_raw_voltage(&dac, DAC_A, raw_data);
    eeprom_write_dword(HEATER_1_TO_4_RAW_SETPOINT_ADDR, raw_data);
}

// Sets temperature setpoint of heater 5 (connected to DAC B)
// raw_data - 12 bit DAC raw data for setpoint
void set_heater_5_raw_setpoint(uint16_t raw_data) {
    set_dac_raw_voltage(&dac, DAC_B, raw_data);
    eeprom_write_dword(HEATER_5_RAW_SETPOINT_ADDR, raw_data);
}

// temp - in C
void set_heaters_1_to_4_temp_setpoint(double temp) {
    double res = therm_temp_to_res(temp);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    set_heaters_1_to_4_raw_setpoint(raw_data);
}

// temp - in C
void set_heater_5_temp_setpoint(double temp) {
    double res = therm_temp_to_res(temp);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    set_heater_5_raw_setpoint(raw_data);
}

void init_heaters(){
    uint16_t heater_1_to_4_last_setpoint = eeprom_read_dword(HEATER_1_TO_4_RAW_SETPOINT_ADDR);
    if (heater_1_to_4_last_setpoint == EEPROM_DEF_DWORD){
        heater_1_to_4_last_setpoint = 0;
    }
    uint16_t heater_5_last_setpoint = eeprom_read_dword(HEATER_5_RAW_SETPOINT_ADDR);
    if (heater_5_last_setpoint == EEPROM_DEF_DWORD){
        heater_5_last_setpoint = 0;
    }
    set_heaters_1_to_4_raw_setpoint(heater_1_to_4_last_setpoint);
    set_heater_5_raw_setpoint(heater_5_last_setpoint);
}
