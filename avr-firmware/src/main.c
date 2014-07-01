
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "config.h"

#include "time/time.h"
#include "uart/uart.h"


#ifdef CANBUS_ENABLED
    #include "can_volvo/can_volvo.h"
    #include "canbus/canbus.h"
#endif

#ifdef RTI_ENABLED
    #include "rti/rti.h"
#endif

#ifdef MELBUS_ENABLED 
    #include "melbus/melbus.h"
#endif



int main(void) {
    uart_init();
//    time_init();
#ifdef RTI_ENABLED
    printf("#RTI init\n");
    rti_init();
#endif

#ifdef CANBUS_ENABLED
    printf("#CAN init\n");
    can_volvo_init();
#endif

    printf("#Init done.\n\n");

#ifdef MELBUS_ENABLED
    melbus_init();
#endif


    while (1) {
#ifdef RTI_ENABLED
        rti_step();
#endif

#ifdef CANBUS_ENABLED
        can_volvo_step();        
#endif

#ifdef MELBUS_ENABLED
        melbus_step();
#endif
    }
}
