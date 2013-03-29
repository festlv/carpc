

#include <avr/io.h>

#include "canbus/canbus.h"
#include "uart/uart.h"
#include <stdio.h>
#include <util/delay.h>
#include "rti/rti.h"
#include "time/time.h"

uint8_t tmp;
char tempchar;
CAN_MESSAGE rx, tx;

int main(void) {
    uart_init();
    canbus_init();
    time_init();
    rti_init();
    printf("Init done.\n\n");
    rti_enable_screen();
    rx.id = 0x95;
    rx.data[0] = '1';
    rx.data[1] = '2';
    rx.data[2] = '3';

    rx.data[3] = '4';
    rx.data[4] = '5';
    rx.data[5] = '6';
    rx.data[6] = '7';
    rx.data[7] = '8';

    rx.data_len = 8;
    rx.ext = 1;
    rx.extended_id = 0b10111111111111111;
    canbus_write_tx_buffer(0, &rx);

    while (1) {
  		if (canbus_read_rx_buffer(0, &tx)>0) {
  			printf("Received CAN message\n");
  			printf("ID: %x\n", tx.id);
  			printf("Data: %s\n", (char*)tx.data);
  			printf("Len: %u\n", tx.data_len);
  			printf("Ext: %lx\n", tx.extended_id);
  			printf("Ext flag: %u\n", tx.ext);
  		} else if (canbus_read_rx_buffer(1, &tx)>0) {
  			printf("Received CAN message\n");
  			printf("ID: %x\n", tx.id);
  			printf("Data: %s\n", (char*)tx.data);
  			printf("Len: %u\n", tx.data_len);
  			printf("Ext: %x\n", tx.extended_id);
  			printf("Ext flag: %u\n", tx.ext);
  		}
  		_delay_ms(1000);
    }
}   
