#include <stdint.h>
#include <avr/io.h>

#define CAPTURE_USE_TIMER1       // TODO: ICP1 is what pin?
#define TIMER_OVERFLOW_VECTOR  TIMER1_OVF_vect
#define TIMER_CAPTURE_VECTOR   TIMER1_CAPT_vect
#define FREQMEASURE_BUFFER_LEN 12

void init_freq_measure(void);
void end_freq_measure(void);

uint32_t read_freq_measure(void);
uint8_t available(void);

float convert_count_to_freq(uint32_t);
float convert_count_to_freq_ns(uint32_t);
