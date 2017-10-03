#include "main.h"

#define SIZE 8;

// void package_sensor_data(uint8_t *data){
//
//
// }


int main (void)
{
	uint8_t data[SIZE];

	sensor_setup();
	init_uart();
	print("\n");
	print("UART Initialized\n");
	// Queue q = initQueue();
	// Queue *command = &q;
	//package_sensor_data(&data);
	//can_send_message(data, SIZE, )

	while(1){
		print("Temperature:\t%d\n", ((int)(read_temperature() * 100)));
		print("Humidity:\t%d\n", (int)(read_humidity() * 100));
	}
}
