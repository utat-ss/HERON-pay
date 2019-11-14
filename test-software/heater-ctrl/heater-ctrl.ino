#define THM1 A0
#define THM2 A1
#define THM3 A2
#define THM4 A3
#define THM5 A4

#define HEAT1 2
#define HEAT2 3
#define HEAT3 4
#define HEAT4 5
#define HEAT5 6

#define THERM_RREF 10 //[Kohm]

const double BETA               = 3900;
const double ROOM_TEMP          = 298.15;   // room temperature in Kelvin

int read_val = 0;
double raw_volt = 0.0;
double temp = 0.0;
double rThermistor = 0.0;
double tKelvin = 0.0;
double tCelsius = 0.0;

//the 8 relay module is actually active low

void enable_heater(int heater_num){
  digitalWrite(heater_num, LOW);
}

void disable_heater(int heater_num){
  digitalWrite(heater_num, HIGH);
}

void read_thermistor(int therm_num){
  //datasheet https://www.murata.com/~/media/webrenewal/support/library/catalog/products/thermistor/r03e.ashx?la=en-us
  read_val = analogRead(therm_num);
  //Serial.println(read_val);

  // Arduino Analog Read conversion, ADC pin supplied with 5V, each count division is 5/1024 = 0.0049V
  raw_volt = read_val*4.9; //[mV]
  Serial.println(raw_volt);
  
  rThermistor = THERM_RREF * (5000/raw_volt - 1);
  Serial.println(rThermistor);
  //tKelvin = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(rThermistor / THERM_RREF)));
  tKelvin = 1 / (log(rThermistor/THERM_RREF)/BETA + 1/ROOM_TEMP);
  tCelsius = tKelvin - 273.15;

  Serial.println(tCelsius);
}

void setup() {
  pinMode(THM1, INPUT);
  pinMode(THM2, INPUT);
  pinMode(THM3, INPUT);
  pinMode(THM4, INPUT);
  pinMode(THM5, INPUT);

  pinMode(HEAT1, OUTPUT);
  pinMode(HEAT2, OUTPUT);
  pinMode(HEAT3, OUTPUT);
  pinMode(HEAT4, OUTPUT);
  //pinMode(HEAT5, OUTPUT);

  disable_heater(HEAT1);
  disable_heater(HEAT2);
  disable_heater(HEAT3);
  disable_heater(HEAT4);
  //digitalWrite(HEAT5, LOW);

  Serial.begin(9600);

  enable_heater(HEAT1);
  enable_heater(HEAT2);
  enable_heater(HEAT3);
}

void loop() {
  // put your main code here, to run repeatedly:
  read_thermistor(THM1);
}
