#include "time/time.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile static uint32_t current_microseconds=0;
volatile static uint8_t ovf_counter=0;

void time_init() {
    TCCR2B |= (1<<CS20); //prescaler = 1 
    TIFR2 = 1<<TOV2; //clear pending interrupts
    TIMSK0 = 1<<OCF2B; //enable interrupt on output compare with reg b 
    OCR2B = 80; // - generates 200k ISRs/s, resolution of 5uS
    sei();
}

ISR(TIMER2_COMPB_vect)
{
	current_microseconds+=5;
}

uint32_t time_ms() {
        return current_microseconds/1000;
}
uint32_t time_us() {
	return current_microseconds;
}

