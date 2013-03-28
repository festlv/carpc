

#include <avr/io.h>

#include "canbus/canbus.h"
#include "uart/uart.h"
#include <stdio.h>
#include <util/delay.h>
#include "rti/rti.h"
#include "time/time.h"

uint8_t tmp;
char tempchar;

int main(void) {
    uart_init();
    time_init();
    rti_init();
    printf("Init done.");
    rti_enable_screen();
    while (1) {
    	rti_step();
    }
}   
