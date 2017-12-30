#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <spi/spi.h>

// Thermistor ADC is using COM_RST to bypass the port expander
#define COM_RST         PC6
#define COM_RST_DDR     DDRC
#define COM_RST_PORT    PORTC

#define THERMISTOR_BASE 0b10001100
#define R_REF   9.1            // (KOhm); measured value

void init_thermistor(void);
uint16_t read_thermistor_adc(int channel);
double convert_thermistor_reading(uint16_t adc_reading);
double resistance_to_temp(double res);

/*
###########################################################################
                            TESTING CODE
###########################################################################

    DEPENDENCIES:       <uart/log.h>
    AUTHORS:            Dylan Vogel
    DATE MODIFIED:      2017-12-29

###########################################################################

    FUNCTION:           Prints the raw ADC value, calculated resistance,
                        and calculated temperature to serial

    void thermistor_testing(void){
    	uint16_t data;                 // stores the raw ADC data
    	double resistance, temp;       // stores calculated values

    	print("Running thermistor testing code\n");

    	init_spi();
    	init_thermistor();

    	while(1){
    		data = read_thermistor_adc(0);        // change to desired channel
    		print("Raw ADC value: %lX\n", (uint32_t)(data));

    		resistance = convert_thermistor_reading(data);
    		print("Resistance (Ohm): %u\n", (int)(resistance * 1000));

    		temp = resistance_to_temp(resistance);
    		print("Temperature (mC): %u\n", (int)(temp * 1000));

    		print("\n\n");
    		_delay_ms(1000);
    	}
    }

###########################################################################
*/
