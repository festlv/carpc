

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
uint8_t *id_ptr;

int main(void) {
    uart_init();
    canbus_init();
    time_init();
    rti_init();
    printf("Init done.\n\n");
    rti_enable_screen();
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
    rx.id = 0x616263;
    canbus_write_tx_buffer(0, &rx);
    int8_t buf_num = -1;
    while (1) {
        if (canbus_rx_status(0)) {
            buf_num = 0; 
        } else if (canbus_rx_status(1)) {
            buf_num = 1;
        }

  		if (buf_num!=-1 && canbus_read_rx_buffer(buf_num, &tx) >0 ) {
  			printf("~");
            for (int8_t i=tx.data_len-1;i>=0; i--)
                putchar(tx.data[i]);
            //fwrite(&(tx.id), 4, 1, stdout);
            for (int8_t j=3;j>=0;j--) {
                putchar((char)( *(((uint8_t*)&tx.id)+j)));
            }
            printf(".\n");

        }   
        buf_num = -1;
        _delay_ms(1000);
        
        canbus_write_tx_buffer(0, &rx);
    }
}
