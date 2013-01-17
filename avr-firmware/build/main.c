#include "canbus/canbus.h"
#include "uart/uart.h"
#include <stdio.h>


int main(void) {
	

	canbus_init();
    for (;;) {
    	int key = canbus_read_key();
    	if (key==1) {

    	}
    }
}   
