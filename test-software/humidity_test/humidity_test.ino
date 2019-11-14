//########################################################################################
//
// humidity_test.cpp - Humidity Test Code
// Written by: Adyn Miles
//
// This code is used to monitor the relative humidity inside the payload bay. It is to be
// used with the humidity_sensor library, and uses the HIH-7131 for measurements.
//
//########################################################################################

#include <humidity_sensor.h>
#include <SPI.h>

void setup() {
  Serial.begin(9600);
  SPI.begin();
  Serial.println("Time(s), Relative Humidity(%)");
}


void loop() {
  humidity_sensor humidity_sensor(10);
  uint16_t hum_raw = humidity_sensor.read_hum();
  double hum = humidity_sensor.convert_hum(hum_raw);
  unsigned long int start_time = millis();
  unsigned long int end_time = (start_time + 5000);
  while (millis() < end_time);
  printDouble(ceil(millis() / 1000), 2);
  Serial.print(", ");
  printDouble(hum, 2);
  Serial.print("\n");
  humidity_sensor.end_hum();
}

void printDouble(double val, unsigned int decimals) {

  Serial.print (int(val));  //prints the int part
  Serial.print("."); // print the decimal point
  unsigned int frac;
  if (val >= 0) {
    frac = (val - int(val));
    for (int i = 0; i < decimals+1; i++) {
      frac = frac * 10;
    }
  }
  else {
    frac = (int(val) - val);
    for (int i = 0; i < decimals+1; i++) {
      frac = frac * 10;
    }
  }
  Serial.print(frac, DEC);
}
