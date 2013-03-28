#include "time/time.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t current_microseconds=0;

void time_init() {
    TCCR0A |= (1<<WGM01);
    TIMSK0 |= (1<<OCIE0A); //enable interrupt on output compare with reg A 
    OCR0A= 200; // - generates 200k ISRs/s, resolution of 5uS
    TCCR0B |= (1<<CS01); //prescaler = 1 
    sei();
}

ISR(TIMER0_COMPA_vect)
{
	current_microseconds+=100;
}

uint32_t time_ms() {
        return current_microseconds/1000;
}
uint32_t time_us() {
	return current_microseconds;
}

