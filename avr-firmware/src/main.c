

#include <avr/io.h>
#include "config.h"

#include "can_volvo/can_volvo.h"
#include "canbus/canbus.h"
#include "uart/uart.h"
#include <stdio.h>
#include <util/delay.h>
#include "rti/rti.h"
#include "time/time.h"

uint8_t tmp;
char tempchar;
CAN_MESSAGE tx;
uint8_t *id_ptr;

int main(void) {
    uart_init();
    time_init();
    rti_init();
    can_volvo_init();
    printf("#Init done.\n\n");

    while (1) {
        rti_step();
        can_volvo_step();        
    }
}
