/*

	FILENAME: 			main.c
	DEPENDENCIES:		pex, uart, can, queue, freq_measure, stdint, delay

	DESCRIPTION:		Main functions to be implemented on the PAY 32M1
	AUTHORS:			Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
	DATE MODIFIED:		2017-12-28
	NOTE:
						PLEASE DON'T COMMIT YOUR TESTING CODE
						ONLY COMMIT ACTUAL CHANGES TO MAIN.C

	REVISION HISTORY:

    2018-01-08:     DV: Added CAN id definitions to the header file
		2017-11-20:		SS: Added testing for PAY command queue
		2017-11-17: 	Added "sensor_led_sequence()" as a sanity check
		2017-11-16: 	Created header. Implemented 'separate function for testing' rule

*/

#include "main.h"


void pbTest (){
	print("\npb testing\n");
	PORTB |= _BV(PB3);//pb3
	_delay_ms(10);
	PORTB &= ~(_BV(PB3));//off
}//tetsing for SSM_v1

// RX and TX mob now declared in main.h
void stopMotor(){
	clear_gpio_a(SSM,1);//motor
	clear_gpio_a(SSM,0);
	//clear_gpio_a(SSM,3);//motor
	//clear_gpio_a(SSM,2);
	_delay_ms(10000);
}

void forwardMotor(){
	//set_gpio_a(SSM,3);
	set_gpio_a(SSM,1);
	_delay_ms(20000);
}

void reverseMotor(){//GPA2/this function is faulty
	//set_gpio_a(SSM,2);
	set_gpio_a(SSM, 0);
	_delay_ms(20000);
}

int main (void){
		init_uart();
		print("\n\nUART Initialized\n");

		//data direction register pb3
		//DDRB |= _BV(PB3);//pb3 32M1 on the SSM_v1
    //data direction register b
    DDRB |= _BV(PB4); // set PB4 to an output, |= or equal
    print("Data direction register set"); //debug statment

		//initialize spi, testing for SSM_v1
		//init_spi();

		//initialize
		sensor_setup();
		init_port_expander();
		print("Sensor set\n");

		//GPB6 port expander LED
		set_dir_b(SSM,6,0);
		//setting to output
		//set_dir_a(SSM,2,0);//MF2
		//set_dir_a(SSM,3,0);//MF2
		set_dir_a(SSM,0,0);//MF1
		set_dir_a(SSM,1,0);//MF1

		//initialize motor
		//clear_gpio_a(SSM,2);
		//clear_gpio_a(SSM,3);//MF2
		clear_gpio_a(SSM,0);
		clear_gpio_a(SSM,1);
		int count;
		count = 0;
    while(1){
			//Code for motor testing and sensors
				count += 1;
			 	print("%d, Humidity,", count);
			  print("%d,", (int)(read_humidity() * 100));//humidity
				print("Temperature,");
				print("%d\n",(int)(read_humidity_temp()));//temperature

				//light up LED connected to GPB4 on port expander
        PORTB |= _BV(PB4);//LED
				//print("reverseMotor\n");
				set_gpio_b(SSM,6);//port expander on
				reverseMotor();//20sec
				stopMotor();//10sec

        PORTB &= ~(_BV(PB4));
				//print("forwardMotor\n");
				clear_gpio_b(SSM,6);
				forwardMotor();//20sec
				stopMotor();//10sec
				/* for testing SSM_v1
				pbTest();
				print("\nspi\n");
				send_spi(0b10101010);
				_delay_ms(10);
				*/
    }
}
// currently just sending "Hello!"
void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 7;

}
void data_tx_callback(uint8_t* data, uint8_t* len){
	// Callback specifically for sending housekeeping/sensor data

}


/*
void tx_callback_1(uint8_t* data, uint8_t* len) {
    static uint32_t val_1 = 0;
    if (val_1 % 2 == 0) {
        data[0] = 0x01;
        *len = 1;
    } else {
        *len = 0;
        resume_mob(&tx_mob_2);
    }

    val_1 += 1;
}

void tx_callback_2(uint8_t* data, uint8_t* len) {
    static uint32_t val_2 = 0;
    if (val_2 % 2 == 0) {
        data[0] = 0x02;
        *len = 1;
    } else {
        *len = 0;
        resume_mob(&tx_mob_1);
    }
    val_2 += 1;
}

void rx_callback(uint8_t* data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++){
        print("Data: %u", data[i]);
    }
    print("\n");
}*/

int main (void){
	init_uart();
	print("\n\nUART Initialized\n");
	pay_can_init();
	// cmd_queue = initQueue();
    init_can();

	// Testing bi-directional CAN transfers
    while(1){};
}

// currently just sending "Hello!"
