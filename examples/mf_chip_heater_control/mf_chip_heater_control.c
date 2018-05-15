/* -------------------- UTAT Space Systems -------------------- */

/* -------------------------- Payload ------------------------- */

/*

	FILENAME: 		heater_temp_read.c
	DEPENDENCIES:		analog_temp.h, pex.h

	DESCRIPTION:	Payload Heater PID Temperature Control
	AUTHORS:		Charlotte Moraldo, Joanna Hon, Dylan Vogel
	DATE MODIFIED:	2018-01-27

*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <uart/log.h>
#include <util/delay.h>
#include <stdint.h>
#include "../../src/analog_temp.h"
#include "../../src/pex.h"

#define NOM_TEMP 	36
#define ERR_TEMP  	5
#define WAIT		10000
#define CYCLE 		2000

void run_PID();
void run_cycle_on();
void run_cycle_off();
double print_and_read_temps();

//PID parameters
double Kp=0.1, Ki=0.1, Kd=0.1;
double error=0, previousError=0;
float curr_temp, new_temp, correction;
double P=0, I=0, D=0, PID=0;


int main (void){
	init_uart();
	print("\n\nUART Initialized\n");

	init_port_expander();
	print("Sensor set\n");

	init_thermistor();

	set_dir_a(SSM,5,0); 	//setting GPA5
	clear_gpio_a(SSM,5); 	//turn GPA5 off

	while(1) {
		curr_temp = print_and_read_temps();
		// print("Current temperature: %d\n", (int)(curr_temp));
		print ("\n\n");
		run_PID();
		// print("New desired temperature: %d\n\n", (int)(new_temp));

		correction = new_temp - curr_temp;
		// print("correction: %d\n", (int)(correction));
		//if positive, heater ON
		//if negative, heater OFF

		if (correction > 0)
	    	run_cycle_on();
		if (correction < 0)
	    	run_cycle_off();
		if (correction == 0)
			_delay_ms(WAIT);
	}
}


void run_cycle_on()
{
  	//put the heater ON for an amount of time
  	//depending on the value of variable correction

	// print("Temperature has to increase, heater turned on\n");
	set_gpio_a(SSM,5);

	while(curr_temp < new_temp) {
		_delay_ms(CYCLE);

		curr_temp = print_and_read_temps();
		// print("Current temperature: %d\n", (int)(curr_temp));
		print ("on \n\n");
	}

	clear_gpio_a(SSM,5);
	// print("Desired temperature reached, heater turned off\n");
}


void run_cycle_off()
{
  	//let heater OFF for an amount of time
 	//depending on the value of variable correction

 	// print("Temperature has to decrease, heater turned off\n");
 	clear_gpio_a(SSM,5);

	while(curr_temp > new_temp) {
		_delay_ms(CYCLE);

		curr_temp = print_and_read_temps();
		// print("Current temperature: %d\n", (int)(curr_temp));
		print ("off \n\n");
	}
	// print("Desired temperature reached\n");
}


void run_PID(){
   	error = curr_temp - NOM_TEMP;
   	// print("Error: %d\n", (int)(error));
  	//pos for input>33°, neg for input<33°

   	P = error;
   	I = I + error;
   	D = error-previousError;

   	new_temp = NOM_TEMP - (Kp*P + Ki*I + Kd*D);
   	previousError = error;
}


double print_and_read_temps() {
	uint16_t data;                 // stores the raw ADC data
	double resistance, temp;       // stores calculated values
 	int i = 0;
 	double out_data;
 	for ( i = 0; i < 4; i++) {
		data = read_thermistor_adc(i);
    //print("Raw ADC value: %lX\n", (uint32_t)(data));
    		resistance = convert_thermistor_reading(data, i);
    // print("Resistance (Ohm): %u\n", (double)(resistance));
    // print("Reference Channel: %u\n", (int)(R_REF[i]));

    		temp = resistance_to_temp(resistance);
    		print("Temperature (10 mC): %d\n", (int)(temp * 100));
    		if (i==0) {out_data = temp;}
 	}
 	return out_data;
}
