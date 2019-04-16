#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <adc/adc.h>
#include <util/delay.h>

#include "../../src/motors.h"

#define LED2 0
#define LED1 1

void calibration();
void simultaneous();
void sensor_reading();
void speed_test();
void turn_test();
void adjustment();

//slower stronger torque
//shaft rotates exactly one circle with this period and times
uint8_t period = 100;
uint8_t times = 12;
double sum = 0.0;
uint8_t step = 0;

// PHT1 is channel 11 (ADC pin 7), PHT2 is channel 10 (ADC pin 8)
uint8_t adc_channels[] = {10, 11};
uint8_t calibration_channel_num = sizeof(adc_channels) / sizeof(adc_channels[0]);
float readings[4] = {0.0, 0.0, 0.0, 0.0};
uint8_t sample_size = 100;

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
  if (len == 0) {
      return 0;
  }
  uint8_t count = 0;
  switch (buf[0]) {
      case 'd':
        while (count < 10){
          print("cycle %d, step %d\n",count+1, step+1);
          actuate_motors(period, times, false);
          count += 1;
          step += 1;
        }
        break;
      case 'u':
        while ((count < 5)&&(step > 0)){
          actuate_motors(period, times, true);
          count += 1;
          step -= 1;
        }
        print("Moving up\n");
        break;
      case 'c':
        calibration();
        break;
      case 'r':
        //reset set point and step count
        print("Moving up\n");
        while (step > 0){
          actuate_motors(period, times, true);
          step -= 1;
        }
        sum = 0.0;
        step = 0;
        print("System reset\n");
        break;
      case 's':
        simultaneous();
        break;
      case 'e':
        //reset set point
        sensor_reading();
        break;
      case 't':
        turn_test();
        break;
      case 'k':
        speed_test();
        break;
      default:
        print("Invalid command\n");
        break;
  }
  return 1;
}

/*
this function runs the motor down for one "step". A "step" refers to one full turn of the shaft.
*/
void calibration(){
    print("Actuating: %dms, %d times, going down\n", period, times);
    // for actuate_motors(), forward -> true -> up, backward -> false -> down
    print("Step Raw1 Volt1 Raw2 Volt2\n");
    while(step < 30){
      actuate_motors(period, times, false);
      step += 1;
      _delay_ms(1000);
      for (uint8_t i = 0; i < calibration_channel_num; i++){
        fetch_all_adc_channels(&adc);
        uint8_t channel = adc_channels[i];
        for (uint8_t j = 0; j < sample_size; j++){
          uint16_t raw_data = read_adc_channel(&adc, channel);
          readings[2*i] = readings[2*i] + raw_data;
          double voltage = adc_raw_data_to_raw_vol(raw_data);
          readings[2*i+1] = readings[2*i+1] + voltage;
        }  
      }
      print("%d %f %f %f %f\n",step,(readings[0]/sample_size),(readings[1]/sample_size),(readings[2]/sample_size),(readings[3]/sample_size));
      for (uint8_t j = 0; j < 4; j++){
        readings[j] = 0.0;
      }
    }
}

/*
This function runs the motor down for until the distance set-point. 
Currently the set-point is calculated by averaging the raw data reading from two calibration ADC channels. In the future the set-point might be mapped to a physical distance by some internal mapping function so it's more human-readable and verifiable.
*/
void simultaneous(){
  sum = 0.0;
  // sum is the set-point for now
  while (sum < 4.3){
    //reset after each step
    sum = 0.0;
    for (uint8_t i = 0; i < calibration_channel_num; i++){
    fetch_all_adc_channels(&adc);
    uint8_t channel = adc_channels[i];
    uint16_t raw_data = read_adc_channel(&adc, channel);
    print("Channel %d Raw Data: %d\n", channel, raw_data);
    sum += raw_data;
    }
    print("sum: %f\n", sum);
    sum = sum/2;
    actuate_motors(period,times,false);
    step += 1;
  }
  disable_motors();
  print("motors disabled\n");
}

/*
This function reads the two sensor ADC channels and print out the raw data and mapped voltage. It then returns an array of readings (in raw data format, uint16_t).
*/
void sensor_reading(){
  for (uint8_t i = 0; i < calibration_channel_num; i++){
    fetch_all_adc_channels(&adc);
    uint8_t channel = adc_channels[i];
    print("Channel %d\n", channel);
    uint16_t raw_data = read_adc_channel(&adc, channel);
    readings[i] = raw_data;
    double voltage = adc_raw_data_to_raw_vol(raw_data);
    print("Raw Data: %d, Voltage: %f V\n", raw_data, voltage);
  }
}

/*
This function runs the motor for a range of periods (from 50~200 with a increment of 10) but the same times. This is for calibration purposes.
*/
void speed_test(){
  uint16_t period;
  print("testing with cycles_num %d\n", times);
  for(uint16_t i = 0; i < 160; i = i + 10){
    period = 50 + i;
    print("iter %d, period %d\n", (i/10), period);
    actuate_motors(period, times, true);
    _delay_ms(1000);
  }
}

/*
This function runs the motor for a range of times (from 5~100 with a increment of 5) but the same period. This is for calibration purposes.
*/
void turn_test(){
  uint16_t cycles;
  print("testing with period %d\n", period);
  for(uint16_t i = 0; i < 100; i = i + 5){
    cycles = 5 + i;
    print("iter %d, period %d\n", (i/5), cycles);
    actuate_motors(period, cycles, true);
    _delay_ms(1000);
  }
}

/*
When calling this function, it can adjust the platform to ensure the actuation plate is level. 
This is done by comparing the two sensors on each side and correct the error with a P(ID) feedback loop.
*/
void adjustment(){
    // pre-calibrated control parameters
    /*uint8_t Kp = 10;
    uint8_t Ki = 0;
    uint8_t Kd = 0;
    uint8_t error = 0;
    uint16_t last_error = 0;
    uint16_t integral = 0;
    uint8_t integral_cap = 2;*/

    // assume two sensors on the side greater +/- 1 is consistent
    //uint8_t desired = (sensor1 + sensor2) >> 1;
    //error = desired - actual;

}

int main(void){
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");

    init_pex(&pex);
    print("PEX Initialized\n");

    init_dac(&dac);
    print("DAC Initialized\n");

    init_adc(&adc);
    print("ADC Initialized\n");

    print("Starting test\n");

    init_motors();
    print("Motors Initialized\n");

    set_pex_pin_dir(&pex, PEX_B, LED1, OUTPUT);
    set_pex_pin_dir(&pex, PEX_B, LED2, OUTPUT);

    set_pex_pin(&pex, PEX_B, LED1, 1);
    set_pex_pin(&pex, PEX_B, LED2, 1);
    print("Turn IR LEDs on\n");

    set_uart_rx_cb(key_pressed);

    while(1){};
}
