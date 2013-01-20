#include <avr/io.h>

#include "uart.h"

#ifndef BAUD
#define BAUD 9600 
#endif
#include <util/setbaud.h>

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */ 
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */    
}

void uart_putchar(char c) {
    UDR0 = c;
    loop_until_bit_is_set(UCSR0A, UDRE0);

}

char uart_getchar() {
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}


void uart_putstring(char* c) {
    int i=0;
    while (c[i]!=0) {
        uart_putchar(c[i]);
        i++;
    }
    
}