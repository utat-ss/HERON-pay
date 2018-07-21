#include "heaters.h"

// init is covered by dac_init()

void heaters_set_temp_a(double temp) {
    double resistance = thermis_temp_to_resistance(temp);
    double voltage = thermis_resistance_to_voltage(resistance);
    dac_set_voltage_a(voltage);
}

void heaters_set_temp_b(double temp) {
    double resistance = thermis_temp_to_resistance(temp);
    double voltage = thermis_resistance_to_voltage(resistance);
    dac_set_voltage_b(voltage);
}

void heaters_set_temp_both(double temp) {
    heaters_set_temp_a(temp);
    heaters_set_temp_b(temp);
}
