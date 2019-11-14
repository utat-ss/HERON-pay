//#######################################################################################
//
// thermal_cycling_test.cpp - Humidity Thermal Cycling Test Code
// Written by: Adyn Miles
//
// This code is used to simultaneously measure the temperature and the relative humidity
// inside the payload bay. It is to be used with the humidity_sensor library, and uses the
// HIH-7131 and a 0402 thermistor for measurements.
//
//
// NOTE: Make sure to check the following for temperature measurements every time you run
//       the experiment.
//       1) ROOM_TEMP should reflect the actual ambient temperature at the time of the 
//          experiment.
//       2) RESISTOR_ROOM_TEMP is the corresponding resistance value which can be found in 
//          the thermistor datasheet.
//       3) BALANCE_RESISTOR should match the resistance across the thermistors being used
//          (use a multimeter to verify this). 
//#######################################################################################

#include "humidity_sensor.h"
#include "SPI.h"

// Defining constants for temperature measurements
const int    SAMPLE_NUMBER      = 10;
const double BALANCE_RESISTOR   = 9200.0;
const double MAX_ADC            = 1023.00;
const double BETA               = 3380.0;
const double ROOM_TEMP          = 298.15;   // room temperature in Kelvin
const double RESISTOR_ROOM_TEMP = 10000.0;
double currentTemperature;
double tempTest;
unsigned long start_time;
unsigned long end_time;
unsigned long time_count = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  Serial.println("Time(s), Temperature(C), Relative Humidity(%)");  
}

void loop() {
  // Initializing the humidity sensor measurement
  humidity_sensor humidity_sensor(10);
  // Taking measurements from the humidity sensor
  uint16_t hum_raw = humidity_sensor.read_hum();
  double hum = humidity_sensor.convert_hum(hum_raw);
  int val = 0;
  // Taking measurements from the temperature sensor
  // Take measurement every 5 seconds, just record time interval as integer to prevent overflow
  currentTemperature = readThermistor();
  unsigned long int start_time = millis();
  unsigned long int end_time = (start_time + 5000);
  while (millis() < end_time);
  Serial.print(time_count);
  time_count = time_count + 1;
  Serial.print(", ");
  //val = analogRead(A0);
  printDouble(currentTemperature, 2); 
  Serial.print(", ");
  printDouble(hum, 2);
  Serial.print("\n"); 
  humidity_sensor.end_hum();

}


// Takes in raw readings from the thermistor, averages them over a given sample number,
// then converts the readings to celsius data using the conversion formula in the 
// datasheet. 
double readThermistor(){
  int     numThermistors = 1;
  double  rThermistor;
  double  tKelvin;
  double  tCelsius;
  int  adcAverage;
  double  someArray;
  int     adcSamples;
  int     thermistors[SAMPLE_NUMBER] = {};
  //double* tempArrPointer;
  //int*    tempArrIntPointer;
    
  //for (int pin = 0; pin < numThermistors; pin++) {
    for (int i = 0; i < SAMPLE_NUMBER; i++) {
      thermistors[SAMPLE_NUMBER] = analogRead(A0); 
      delay(10);
    } 
  //}

  //for (int pin = 0; pin < numThermistors; pin++) {
    for (int i = 0; i < SAMPLE_NUMBER; i++) { 
      adcAverage += thermistors[SAMPLE_NUMBER];
      delay(10);
    }
    adcAverage /= SAMPLE_NUMBER; 
    rThermistor = BALANCE_RESISTOR * ((MAX_ADC/adcAverage) - 1);
    tKelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(rThermistor / RESISTOR_ROOM_TEMP)));
    tCelsius = tKelvin - 273.15;
  //} 
  return tCelsius;
  //tempArrPointer = tCelsius;
  //return tempArrPointer; 
}

// Prints values with the number of decimal places determined by the precision variable.
// NOTE: Precision is 1 followed by the number of zeros for the desired number of decimal places.
void printDouble(double val, unsigned int precision) {
  // Prints the integer part
  Serial.print(int(val));
  Serial.print(".");
  unsigned int frac;
  if (val >= 0) { 
    frac = (val - int(val))*precision;
  }
  else {
    frac = (int(val)-val)*precision;
  }
  Serial.print(frac, DEC); 
}
