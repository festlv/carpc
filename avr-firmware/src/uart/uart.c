#include <avr/io.h>

#include "uart.h"
#ifndef BAUD
#define BAUD 115200 
#endif

//ignore the warning about baud rate
#pragma GCC diagnostic warning "-Wcpp"
#pragma GCC diagnostic push
#include <util/setbaud.h>
#pragma GCC diagnostic pop

#define UNUSED(x) (void)(x)

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);




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

    stdout = stdin = &uart_str;
}

int uart_putchar(char c, FILE *stream) {
    UNUSED(stream);

    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

int uart_getchar(FILE *stream) {
    UNUSED(stream);

    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}


void uart_putstring(char* c) {
    int i=0;
    while (c[i]!=0) {
        uart_putchar(c[i], NULL);
        i++;
    }
    
}
