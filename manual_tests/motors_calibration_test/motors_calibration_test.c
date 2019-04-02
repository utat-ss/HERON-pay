#include <pex/pex.h>
#include <spi/spi.h>
#include <uart/uart.h>
#include <adc/adc.h>
#include <util/delay.h>

#include "../../src/motors.h"

#define LED2 0
#define LED1 1

void calibration();
void stop(); // not used
void sensor_reading();
uint8_t period = 100;
uint8_t times = 12; //shaft exactly rotates one circle

// (channels 10 and 11 are something else - motor positioning sensors)
// PHT1 is channel 11, PHT2 is channel 10
// uint8_t adc_channels[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
uint8_t adc_channels[] = {10, 11};
uint8_t calibration_channel_num = sizeof(adc_channels) / sizeof(adc_channels[0]);

uint8_t key_pressed(const uint8_t* buf, uint8_t len) {
  if (len == 0) {
      return 0;
  }
  // forward -> up
  // backward -> down
  uint8_t count = 1;
  switch (buf[0]) {
      case 'w':
        while (count < 3){
          print("cycle %d\n",count);
          calibration();
          count += 1;
        }
        break;
      case 's':
        while (count < 3){
          actuate_motors(period, times, true);
          count += 1;
        }
        break;
      case 'r':
        sensor_reading();
        break;
      default:
        print("Invalid command\n");
        break;
  }
  return 1;
}

// this function runs the motor down continuously until user press stop
// it counts the "step" the motors need to go from top to MF chip
void calibration(){
    print("Actuating: %dms, %d times, going down\n", period, times);
    actuate_motors(period, times, false);

    sensor_reading();
}

void stop(){
    disable_motors();
    print("motors disabled\n");
}

void sensor_reading(){
  for (uint8_t i = 0; i < calibration_channel_num; i++){
    fetch_all_adc_channels(&adc);
    uint8_t channel = adc_channels[i];
    print("Channel %d\n", channel);
    uint16_t raw_data = read_adc_channel(&adc, channel);
    double voltage = adc_raw_data_to_raw_vol(raw_data);
    print("Raw Data: %d, Voltage: %f V\n", raw_data, voltage);
  }
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

    print("Turn IR LEDs on\n");
    set_pex_pin(&pex, PEX_B, LED1, 1);
    set_pex_pin(&pex, PEX_B, LED2, 1);

    set_uart_rx_cb(key_pressed);

    /*while (1) {
        print("Turn IR LEDs on")
        set_pex_pin(&pex, PEX_B, LED1, 1);
        set_pex_pin(&pex, PEX_B, LED2, 1);

        for (uint8_t j = 0; j < 3; j++){
          print("Actuating: 100ms, 50 times, backward\n");
          actuate_motors(100, 50, false);
          print("Waiting\n");
          _delay_ms(2000);

          fetch_all_adc_channels(&adc);
          uint8_t channel = 11;
          print("Channel %d\n", channel);
          uint16_t raw_data = read_adc_channel(&adc, channel);
          double voltage = adc_raw_data_to_raw_vol(raw_data);
          print("Voltage: %f V\n", voltage);
        }*/

    while(1){};
        /*for (uint8_t i = 0; i < calibration_channel_num; i++) {
            uint8_t channel = adc_channels[i];
            print("Channel %d\n", channel);
            uint16_t raw_data = read_adc_channel(&adc, channel);
            double voltage = adc_raw_data_to_raw_vol(raw_data);
            // Print temperature/resistance/voltage of thermistors
            print("Voltage: %f V\n", voltage);
            _delay_ms(1500);
        }*/

        //count += 1;
}
