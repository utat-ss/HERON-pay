/*

	FILENAME: 			analog_temp.c
	DEPENDENCIES:		spi

	DESCRIPTION:		Functions for measuring thermistor temperature
	AUTHORS:			Dylan Vogel, James Sheridan
	DATE MODIFIED:	    2017-12-28
	NOTE:

	REVISION HISTORY:

        2017-12-28      DV: Created file, implemented lookup table.

*/

#include "analog_temp.h"

double resistance_to_temp(double res){
    int i;
    double diff, slope, temp;

    // Resintances are stored in kilo-ohms
    static const double resistances[34] PROGMEM = {
        195.652,    148.171,    113.347,    87.559, 68.237,
        53.650,     42.506,     33.892,     27.219, 22.021,
        17.926,     14.674,     12.081,     10.000, 8.315,
        6.948,      5.834,      4.917,      4.161,  3.535,
        3.014,      2.586,      2.228,      1.925,  1.669,
        1.452,      1.268,      1.110,      0.974,  0.858,
        0.758,      0.672,      0.596,      0.531
    };
    static const int temperatures[34] PROGMEM = {
        -40,    -35,    -30,    -25,    -20,
        -15,    -10,    -5,     0,      05,
        10,     15,     20,     25,     30,
        35,     40,     45,     50,     55,
        60,     65,     70,     75,     80,
        85,     90,     95,     100,    105,
        110,    115,    120,    125
    };

    for (i = 0; i < 34, i++){
        if ((res - resistances[i]) >= 0){
            diff = res - resistances[i-1];  //should be negative
            slope = (double)(temp[i] - temp[i-1]) / (resistances[i] - resistances[i-1]);

            temp = temp[i-1] + (diff * slope);
            break;
        }
    }
    return temp;
}
