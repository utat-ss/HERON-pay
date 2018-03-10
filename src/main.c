/*

    FILENAME:            main.c
    DEPENDENCIES:        pex, uart, can, queue, freq_measure, stdint, delay

    DESCRIPTION:        Main functions to be implemented on the PAY 32M1
    AUTHORS:            Dylan Vogel, Shimi Smith, Bruno Almeida, Russel Brown
    DATE MODIFIED:      2017-12-28
    NOTE:
    PLEASE DON'T COMMIT YOUR TESTING CODE
    ONLY COMMIT ACTUAL CHANGES TO MAIN.C

    REVISION HISTORY:

    2018-01-08:     DV: Added CAN id definitions to the header file
    2017-11-20:     SS: Added testing for PAY command queue
    2017-11-17:     Added "sensor_led_sequence()" as a sanity check
    2017-11-16:     Created header. Implemented 'separate function for testing' rule

*/

#include "main.h"
#include <can/packets.h>
#include <stdlib.h>

// function prototypes for functions used only in main.c
void pay_can_init(void);
uint8_t handle_cmd(uint8_t*);
uint8_t handle_hk_req(uint8_t* rx_data, uint8_t* tx_data);
uint8_t handle_sci_req(uint8_t* rx_data, uint8_t* tx_data);

// TODO - might need some other queue to store the length of each data array
// Currently, just send 8 bytes even though we only use the first 5
queue_t *cmd_queue;
queue_t *data_tx_queue;

int main (void){
    init_uart();
    print("\nUART Initialized\n");
    pay_can_init();
    print("CAN MOBs Initialized\n");
    init_can();
	print("CAN Initialized\n");

    sensor_setup();
    print("Sensors Initialized\n");

    // initialize the command queue
    init_queue(cmd_queue);
    init_queue(data_tx_queue);

    uint8_t data[8];  // the data currently being processed

    while(1){
        if (!is_empty(cmd_queue)) {
            dequeue(cmd_queue, data);
            handle_cmd(data);
            // proccess the command
            // this will eventually involve writing onto the data_tx_queue
        }

        if (!is_empty(data_tx_queue)) {
            resume_mob(&data_tx_mob); // resuming the mob attempts
            // to dequeue from the data_tx_queue
        }
    }
}

void pay_can_init(void){
    // TODO: Consider adding print statements which show the data loaded into
    // the TX MObs, for debugging.
    // init_rx_mob(&status_rx_mob);
    // init_tx_mob(&status_tx_mob);
    // init_tx_mob(&cmd_tx_mob);
    init_rx_mob(&cmd_rx_mob);
    init_tx_mob(&data_tx_mob);
}


// When resume_mob(mob name) is called,
// it will start the MOB and trigger an interrupt (callback function) to get the data to transmit.
// If the length of the new data is 0, it will pause the mob.
// New data must already be available to send instead of waiting for it
// (use a global/static variable or similar)

// MOB 0
// For heartbeat
void status_rx_callback(uint8_t* data, uint8_t len){
    // Resume the MOb to send the status back
    // resume_mob(&status_tx_mob);
}

// MOB 1
// For heartbeat
void status_tx_callback(uint8_t* data, uint8_t* len){

}

// MOB 2
// For later science requests?
void cmd_tx_callback(uint8_t* data, uint8_t* len){

}

// MOB 3
// this is for recieving commands
// Receives commands and puts them in cmd_queue
void cmd_rx_callback(uint8_t* data, uint8_t len){
	print("\ncmd_rx_callback()\n");
	print("Received CAN Message: ");
	for (int i = 0; i < len; i++) {
		print("0x%02x ", data[i]);
	}
	print("\n");

    print("Packet: 0x%02x\n", data[0]);
    print("Field: 0x%02x\n", data[1]);

    // TODO is it always 8 bytes of data?
    enqueue(cmd_queue, data);

	print("Enqueued RX Message\n");
}

// MOB 5
void data_tx_callback(uint8_t* data, uint8_t* len) {
	print("\ndata_tx_callback()\n");

    if (!is_empty(data_tx_queue)) {
        uint8_t tx_data[8] = {0};
        dequeue(data_tx_queue, tx_data);
        *len = 8;   // TODO - check length

        // TODO - can the intermediate variable tx_data be eliminated?
        for (int i = 0; i < 8; i++) {
            data[i] = tx_data[i];
        }

		print("Transmitting CAN Message: ");
        for (int i = 0; i < *len; i++) {
            print("0x%02x ", data[i]);
        }
        print("\n");

        print("Packet: 0x%02x\n", data[0]);
        print("Field: 0x%02x\n", data[1]);
        print("Data: ");
        for (int i = 2; i < *len; i++) {
            print("0x%02x ", data[i]);
        }
        print("\n");
    } else {
        // otherwise, set len to zero, because we have no data to send
        *len = 0;

		print("No CAN message to send\n");
    }
}

// TODO - what is the return value?
// Answer: returns 1 for success, 0 for failure
uint8_t handle_cmd(uint8_t* rx_data){
    uint8_t len = 8;    // TODO check

	print("\nhandle_cmd()\n");

	// Data to send
    uint8_t tx_data[8];

	// Repeat the identification bytes
	tx_data[0] = rx_data[0];
	tx_data[1] = rx_data[1];

    switch (rx_data[0]){
        case PAY_HK_REQ:
            handle_hk_req(rx_data, tx_data);
            break;
        case PAY_SCI_REQ:
            handle_sci_req(rx_data, tx_data);
            break;
        default:
            break;
    }

	enqueue(data_tx_queue, tx_data);

    print("Enqueued TX data\n");
    for (int i = 0; i < len; i++) {
        print("0x%02x ", tx_data[i]);
    }
    print("\n");

    print("Packet: 0x%02x\n", tx_data[0]);
    print("Field: 0x%02x\n", tx_data[1]);
    print("Data: ");
    for (int i = 2; i < len; i++) {
        print("0x%02x ", tx_data[i]);
    }
    print("\n");

    return 0;
}

// Handles a housekeeping request - temperature, humidity, or pressure
// Loads the sensor data onto tx_data
// TODO - what is this supposed to return?
uint8_t handle_hk_req(uint8_t* rx_data, uint8_t* tx_data){
	print("\nhandle_hk_req()\n");

	// Need to declare this here because of the error
	// "a label can only be part of a statement and a declaration is not a statement"
	// (happens when a variable is declared inside the case statement)
	uint16_t temp;
	uint32_t pressure;
	uint32_t humidity;

    switch (rx_data[1]){
        case PAY_TEMP_1:
			// temp = read_raw_temperature();
			temp = rand() % ((uint32_t) 1 << 14);	// dummy value
            print("PAY_TEMP_1: %x\n",temp);

			tx_data[2] = 0;	// zero padded
            tx_data[3] = (temp >> 8);
            tx_data[4] = (temp && 0x00FF);
            break;


		case PAY_PRES_1:
			// TODO - why does this need temperature?
            // TODO - temperature needed for pressure calculation
			// have to send both the pressure and the temperature of the sensor
            // put data as <PRESSURE><TEMPERATURE>
			// uint32_t temp = read_raw_pressure_temp();

			// 24 bit value
            // pressure = read_raw_pressure();
			pressure = rand() % ((uint32_t) 1 << 24);    // dummy value
			print("PAY_PRES_1: %x\n", pressure);

            tx_data[2] = (pressure >> 16) & 0xFF;
            tx_data[3] = (pressure >> 8) & 0xFF;
            tx_data[4] = pressure & 0xFF;

            // data[4] = (temp >> 24) & 0xFF;
            // data[5] = (temp >> 16) & 0xFF;
            // data[6] = (temp >> 8) & 0xFF;
            // data[7] = temp & 0xFF;

            break;


        case PAY_HUMID_1:
			// read_raw_humidity() gives 16 bits of humidity followed by 16 bits of temperature,
			// but humidity is a 14 bit value
			// humidity = (read_raw_humidity() >> 16) & 0x3FFF;
			humidity = rand() % (1 << 14);	// dummy value
			print("PAY_HUMID_1: %x\n", humidity);

			tx_data[2] = 0;	// zero padded
			tx_data[3] = (humidity >> 8) & 0xFF;
			tx_data[4] = humidity & 0xFF;

            // data[0] = (humidity >> 24) & 0xFF;
            // data[1] = (humidity >> 16) & 0xFF;
            // data[2] = (humidity >> 8) & 0xFF;
            // data[3] = humidity & 0xFF;
            break;


		case PAY_MF_TEMP_1:
            // temp = read_thermistor_adc(0); // channel 0
            temp = rand() % ((uint32_t) 1 << 12);	// dummy value
            print("PAY_MF_TEMP_1: %x\n",temp);

            tx_data[2] = 0;	// zero padded
            tx_data[3] = (temp >> 8);
            tx_data[4] = (temp && 0x00FF);

            break;

	    case PAY_MF_TEMP_2:
            // temp = read_thermistor_adc(1); // channel 1
            temp = rand() % ((uint32_t) 1 << 12);	// dummy value
            print("PAY_MF_TEMP_2: %x\n",temp);

            tx_data[2] = 0;	// zero padded
            tx_data[3] = (temp >> 8);
            tx_data[4] = (temp && 0x00FF);

	        break;

	    case PAY_MF_TEMP_3:
            // temp = read_thermistor_adc(2); // channel 2
            temp = rand() % ((uint32_t) 1 << 12);	// dummy value
            print("PAY_MF_TEMP_3: %x\n",temp);

            tx_data[2] = 0;	// zero padded
            tx_data[3] = (temp >> 8);
            tx_data[4] = (temp && 0x00FF);

	        break;

        default:
            return 1;
    }

	return 0;
}

// Handles a science request - optical density or fluorescence
// Loads the sensor data onto tx_data
// TODO - what is this supposed to return?
uint8_t handle_sci_req(uint8_t* rx_data, uint8_t* tx_data){
	print("\nhandle_sci_req()\n");
	// TODO - differentiate OD and FL measurements

    // Isolate last 6 bits
	uint8_t well_num = rx_data[1] & 0x3F;

    int channel;
    int LED;

	// Check 6 bytes for well number, poll appropriate sensor
	// TODO - add more sensors
	switch (well_num) {
        // TEMD
        case 0:
        	channel = 5;
        	LED = LED_2;

        // SFH
        case 1:
        	channel = 6;
        	LED = LED_3;
	}

    // TODO - activate LED
	// uint32_t reading = read_ADC_channel(channel);

	uint32_t reading = rand() % ((uint32_t) 1 << 24);  // dummy value
    print("Channel: %d\n", channel);
    print("LED: %d\n", LED);
	print("Well #%d: 0x%x\n", well_num, reading);
    
	tx_data[2] = (reading >> 16) & 0xFF;
	tx_data[3] = (reading >> 8) & 0xFF;
	tx_data[4] = reading & 0xFF;

	return 0;
}
