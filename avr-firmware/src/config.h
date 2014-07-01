#ifndef __CONFIG__H
#define __CONFIG__H


#include <avr/io.h>

#define CANBUS_ENABLED 1

#define RTI_ENABLED 1

//#define MELBUS_ENABLED 1

#define RTI_SERIAL_TX_DDR DDRB
#define RTI_SERIAL_TX_PORT PORTB
#define RTI_SERIAL_TX_BIT PB0


#define CANBUS_RESET_DDR DDRB
#define CANBUS_RESET_PORT PORTB
#define CANBUS_RESET_BIT PB1

#define CANBUS_CS_DDR DDRB
#define CANBUS_CS_PORT PORTB
#define CANBUS_CS_BIT PB2

#define RELAY_DDR DDRD
#define RELAY_PORT PORTD
#define RELAY_BIT PD6

//define this to receive CANBUS data via serial console
#define CANBUS_LOGGING_MODE

#endif
