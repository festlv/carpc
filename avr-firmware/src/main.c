

#include <avr/io.h>

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
    can_volvo_init();
    time_init();
    rti_init();
    printf("Init done.\n\n");
    rti_enable_screen();
    tx.id = 0x040066;
    tx.ext = 1;
    tx.data_len = 8;
    tx.data[0]=0x40;
    tx.data[1]=0x00;
    tx.data[2]=0xf5;
    tx.data[3]=0x01;
    tx.data[4]=0x84;
    tx.data[5]=0xf4;
    tx.data[6]=0x07;
    tx.data[7]=0x00;
    canbus_write_tx_buffer(0, &tx);
    while (1) {
        rti_step();
        can_volvo_step();        
    }
}
