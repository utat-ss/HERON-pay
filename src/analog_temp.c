/*

	FILENAME: 			main.c
	DEPENDENCIES:		adc, pex, spi, uart

	DESCRIPTION:		Main functions to be implemented on the PAY 32M1
	AUTHORS:				Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
	DATE MODIFIED:	2017-11-16
	NOTE:
								* Please adhere to the 'separate function for testing' rule
	BUG:
								* Compiling throws a warning that F_CPU has been previously defined
									by a file associated with CAN. Can #CDH sort this out?

	REVISION HISTORY:

        2017-12-12:     Added basic CAN message handling and command queue implementation
        2017-11-21:     Added additional ADC testing functions for BioStack I
		2017-11-17: 	Added "sensor_led_sequence()" as a sanity check
		2017-11-16: 	Created header. Implemented 'separate function for testing' rule

*/
