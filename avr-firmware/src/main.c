#include <avr/io.h>

#include "canbus/canbus.h"
#include "uart/uart.h"

#include <util/delay.h>


uint8_t tmp;
char tempchar;

int main(void) {
	uart_init();
	if (canbus_init()!=0) {
		uart_putstring("Init failed!");
	}
	DDRC |= (1<<PC3);
	PORTC |=(1<<PC3);

	uart_putstring("Init successful!");
    for (;;) {
    	_delay_ms(1000);
    	tmp = canbus_status();
    	uart_putchar((char) tmp);
    	
    	PORTC ^= (1<<PC3);


    	
    }
}   
