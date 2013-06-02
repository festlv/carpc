#ifndef __CONFIG__H
#define __CONFIG__H


#include <avr/io.h>


#define RTI_SERIAL_TX_DDR DDRB
#define RTI_SERIAL_TX_PORT PORTB
#define RTI_SERIAL_TX_BIT PB0


#define CANBUS_RESET_DDR DDRC
#define CANBUS_RESET_PORT PORTC
#define CANBUS_RESET_BIT PC1

#define CANBUS_CS_DDR DDRC
#define CANBUS_CS_PORT PORTC
#define CANBUS_CS_BIT PC0

#define RELAY_DDR DDRC
#define RELAY_PORT PORTC
#define RELAY_BIT PC3

//define this to receive CANBUS data via serial console
//#define CANBUS_LOGGING_MODE 1

#endif
