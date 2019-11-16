//FUTURE TO-DO: discuss global variables usage
#define THM1 A0
#define THM2 A1
#define THM3 A2
#define THM4 A3
#define THM5 A4
#define THM_NUM 5

#define HEAT1 2
#define HEAT2 3
#define HEAT3 4
#define HEAT4 5
#define HEAT5 6

#define THERM_RREF 9.96 //[Kohm]
#define BETA 3900 // factor from datasheet
#define ROOM_TEMP 298.15 // room temperature in Kelvin
#define CYCLE 10 //[sec]

//set point definition
#define MID_SET 30
#define TOP_SET 28
#define BOT_SET 28

int read_val = 0;
double raw_volt = 0.0;
double temp = 0.0;
double rThermistor = 0.0;
double tKelvin = 0.0;
double tCelsius = 0.0;
float thermistors[5];
unsigned int cycle_count = 0;

//the 8 relay module is actually active low

void enable_heater(int heater_num){
  digitalWrite(heater_num, LOW);
}

void disable_heater(int heater_num){
  digitalWrite(heater_num, HIGH);
}

float read_thermistor(int therm_num){
  //datasheet https://www.murata.com/~/media/webrenewal/support/library/catalog/products/thermistor/r03e.ashx?la=en-us
  int therm;
  //TO-DO: pass in correct analog pins mapping, get rid of the switch statement
  switch(therm_num){
    case 1:
      therm = THM1;
      break;
    case 2:
      therm = THM2;
      break;
    case 3:
      therm = THM3;
      break;
    case 4:
      therm = THM4;
      break;
    case 5:
      therm = THM5;
      break;
    default:
      Serial.println("invalid thermistor number");
      return 0.0;
  }
  read_val = analogRead(therm);
  //Serial.println(read_val);

  // Arduino Analog Read conversion, ADC pin supplied with 5V, each count division is 5/1024 = 0.0049V
  raw_volt = read_val*4.9; //[mV]
  //Serial.println(raw_volt);
  
  rThermistor = THERM_RREF * (5000/raw_volt - 1);
  //Serial.println(rThermistor);
  tKelvin = 1 / (log(rThermistor/THERM_RREF)/BETA + 1/ROOM_TEMP);
  tCelsius = tKelvin - 273.15;

  //Serial.println(tCelsius);
  return tCelsius;
}

void read_thermistors_arr(float* thermistors){
  for(int i = 0; i < THM_NUM; i++){
    thermistors[i] = read_thermistor(i+1);
  }
}

void print_results(unsigned int cycle_count, float middle, float top, float bottom){
  Serial.print(cycle_count);
  Serial.print(", ");
  Serial.print(middle,5);
  Serial.print(", ");
  Serial.print(top,5);
  Serial.print(", ");
  Serial.print(bottom,5);
  Serial.print("\n");
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
  pinMode(HEAT5, OUTPUT);

  disable_heater(HEAT1);
  disable_heater(HEAT2);
  disable_heater(HEAT3);
  disable_heater(HEAT4);
  disable_heater(HEAT5);

  Serial.begin(9600);

  Serial.print("Cycle # | Middle | Top | Bottom \n");
}

void loop() {
  // put your main code here, to run repeatedly:
  read_thermistors_arr(thermistors);

  float middle_avg = (thermistors[1] + thermistors[4])/2;
  float top_avg = (thermistors[1]+thermistors[2])/2;
  float bottom_avg = (thermistors[3] + thermistors[4] + thermistors[5])/3;

  print_results(cycle_count, middle_avg, top_avg, bottom_avg);

  //TO-DO: more sophisticated control loop, be careful of oscillating behaviour
  if (middle_avg < MID_SET){
    enable_heater(HEAT3);
    if(top_avg < TOP_SET){
      enable_heater(HEAT1);
    }else{
      disable_heater(HEAT1);
    }

    if(bottom_avg < BOT_SET){
      enable_heater(HEAT2);
    }else{
      disable_heater(HEAT2);
    }
  }else{
    disable_heater(HEAT3);
  }

  
  //update every x seconds
  //FUTURE TO-DO: try longer time scale, around minutes will be good
  delay(CYCLE*1000);
  cycle_count += 1;
}
