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

// function prototypes for functions used only in main.c
void pay_can_init(void);
uint8_t handle_cmd(uint8_t*);
uint8_t handle_hk_req(uint8_t page, uint8_t* data);
uint8_t handle_hk_sensor_req(uint8_t sensor_id, uint8_t* data);


queue_t *cmd_queue;

int main (void){
	init_uart();
	print("\n\nUART Initialized\n");
	pay_can_init();
  init_can();

	sensor_setup();

	// initialize the command queue
	init_queue(cmd_queue);

	uint8_t data[8];  // the data currently being processed

  while(1){
		if (!is_empty(cmd_queue)){
			dequeue(cmd_queue, data);

			// proccess the command
			handle_cmd(data);
		}
	};
}

void pay_can_init(void){
	// TODO: Consider adding print statements which show the data loaded into
	// the TX MObs, for debugging.

	init_rx_mob(&status_rx_mob);
	init_tx_mob(&status_tx_mob);
	init_tx_mob(&cmd_tx_mob);
	init_rx_mob(&cmd_rx_mob);
	init_tx_mob(&data_tx_mob);
}

void status_rx_callback(uint8_t* data, uint8_t len){

	// Resume the MOb to send the status back
	resume_mob(&status_tx_mob);
}
void status_tx_callback(uint8_t* data, uint8_t* len){

}
void cmd_tx_callback(uint8_t* data, uint8_t* len){

}

// this is for recieving commands
void cmd_rx_callback(uint8_t* data, uint8_t len){
	// TODO is it always be 8 bytes of data?
	enqueue(cmd_queue, data);
}

void data_tx_callback(uint8_t* data, uint8_t* len){
	// Callback specifically for sending housekeeping/sensor data

}

uint8_t handle_cmd(uint8_t* data){
	uint8_t result[8];  // data we are sending back

	switch (data[0]){
		case PAY_HK_REQ:
			handle_hk_req(data[1], result);  // NOTE can't send this all in one CAN frame
			break;
		case PAY_HK_SENSOR_REQ:
			handle_hk_sensor_req(data[1], result);
			break;
		default:
			return 1;
	}

	return 0;
}

// get temp, humidity and pressure
uint8_t handle_hk_req(uint8_t page, uint8_t* data){
		switch (page){
			case 0:
				return handle_hk_sensor_req(PAY_TEMP_1, data);
				break;
			case 1:
				return handle_hk_sensor_req(PAY_HUMID_1, data);
				break;
			case 2:
				return handle_hk_sensor_req(PAY_PRES_1, data);
				break;
			default:
				return 1;
		}
}

// loads the sensor data of the sensor with id sensor_id into the array data
uint8_t handle_hk_sensor_req(uint8_t sensor_id, uint8_t* data){
	switch (sensor_id){
		case PAY_TEMP_1:
		{
			uint16_t temp = read_raw_temperature();
			data[0] = (temp >> 8);
			data[1] = (temp && 0x00FF);
			data[2], data[3], data[4], data[5], data[6], data[7] = 0;
			break;
		}
		case PAY_PRES_1:
		{
			// have to send both the pressure and the temperature of the sensor
			// put data as <PRESSURE><TEMPERATURE>
			uint32_t pressure = read_raw_pressure();
			uint32_t temp = read_raw_pressure_temp();

			data[0] = (pressure >> 24) & 0xFF;
			data[1] = (pressure >> 16) & 0xFF;
			data[2] = (pressure >> 8) & 0xFF;
			data[3] = pressure & 0xFF;

			data[4] = (temp >> 24) & 0xFF;
			data[5] = (temp >> 16) & 0xFF;
			data[6] = (temp >> 8) & 0xFF;
			data[7] = temp & 0xFF;

			break;
		}
		case PAY_HUMID_1:
		{
			uint32_t humidity = read_raw_humidity();
			data[0] = (humidity >> 24) & 0xFF;
			data[1] = (humidity >> 16) & 0xFF;
			data[2] = (humidity >> 8) & 0xFF;
			data[3] = humidity & 0xFF;
			data[4], data[5], data[6], data[7] = 0;
			break;
		}
		case PAY_MF_TEMP_1:
			// TODO talk with Dylan and implement
			break;
		case PAY_MF_TEMP_2:
			// TODO talk with Dylan and implement
			break;
		case PAY_MF_TEMP_3:
			// TODO talk with Dylan and implement
			break;
		default:
		return 1;
	}

	return 0;
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
