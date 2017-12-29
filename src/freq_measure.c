#include "freq_measure.h"

// TODO: lots of implicit initialization, and relying on static
// variables to be zeroed.. not good
static volatile uint32_t buffer_value[FREQMEASURE_BUFFER_LEN];
static volatile uint8_t buffer_head;
static volatile uint8_t buffer_tail;
static uint16_t capture_msw;
static uint32_t capture_previous;
static uint8_t saveTCCR1A, saveTCCR1B;

void capture_init(void) {
	saveTCCR1A = TCCR1A;
	saveTCCR1B = TCCR1B;
	TCCR1B = 0;
	TCCR1A = 0;
	TCNT1 = 0;
	TIFR1 = _BV(ICF1) | _BV(TOV1);
	TIMSK1 = _BV(ICIE1) | _BV(TOIE1);
}

void capture_start(void) {
	TCCR1B = _BV(ICNC1) | _BV(ICES1) | _BV(CS10);
}

uint16_t capture_read(void) {
	return ICR1;
}

uint8_t capture_overflow(void) {
	return TIFR1 & _BV(TOV1);
}

void capture_overflow_reset(void) {
	TIFR1 = _BV(TOV1);
}

void capture_shutdown(void) {
	TCCR1B = 0;
	TIMSK1 = 0;
	TCCR1A = saveTCCR1A;
    TCCR1B = saveTCCR1B;
}

void init_freq_measure(void) {
  capture_init();
	capture_msw = 0;
	capture_previous = 0;
	buffer_head = 0;
	buffer_tail = 0;
	capture_start();
}

// TODO: pick better function names; these aren't very descriptive
uint8_t available(void) {
	uint8_t head, tail;

	head = buffer_head;
	tail = buffer_tail;
	if (head >= tail) return head - tail;
	return FREQMEASURE_BUFFER_LEN + head - tail;
}

uint32_t read_freq_measure(void) {
	uint8_t head, tail;
	uint32_t value;

	head = buffer_head;
	tail = buffer_tail;
	if (head == tail) return 0xFFFFFFFF;
	tail = tail + 1;
	if (tail >= FREQMEASURE_BUFFER_LEN) tail = 0;
	value = buffer_value[tail];
	buffer_tail = tail;
	return value;
}

float convert_count_to_freq(uint32_t count) {
	return (float)F_CPU / (float)count;
}

float convert_count_to_freq_ns(uint32_t count) {
	return (float)count * (1000000000.0f / (float)F_CPU);
}

void end_freq_measure(void) {
	capture_shutdown();
}

// TODO: is this safe? check
ISR(TIMER_OVERFLOW_VECTOR) {
	capture_msw++;
}

ISR(TIMER_CAPTURE_VECTOR) {
	uint16_t capture_lsw;
	uint32_t capture, period;
	uint8_t i;

	// get the timer capture
	capture_lsw = capture_read();
	// Handle the case where but capture and overflow interrupts were pending
	// (eg, interrupts were disabled for a while), or where the overflow occurred
	// while this ISR was starting up.  However, if we read a 16 bit number that
	// is very close to overflow, then ignore any overflow since it probably
	// just happened.
	if (capture_overflow() && capture_lsw < 0xFF00) {
		capture_overflow_reset();
		capture_msw++;
	}
	// compute the waveform period
	capture = ((uint32_t)capture_msw << 16) | capture_lsw;
	period = capture - capture_previous;
	capture_previous = capture;
	// store it into the buffer
	i = buffer_head + 1;
	if (i >= FREQMEASURE_BUFFER_LEN) i = 0;
	if (i != buffer_tail) {
		buffer_value[i] = period;
		buffer_head = i;
	}
}
