#include <avr/io.h>

#include "canbus/canbus.h"
#include "uart/uart.h"
#include <stdio.h>
#include <util/delay.h>
#include <rti/rti.h>
#include "time/time.h"

uint8_t tmp;
char tempchar;

int main(void) {
	uart_init();
	if (canbus_init()!=0) {
        printf("#CAN init failed!");
	}
    rti_init();
    for (;;) {
    	_delay_ms(1000);
        rti_loop();
    	tmp = canbus_status();
    	printf("%x", tmp);


    	
    }
}   
