/** 
* This file contains CANBUS-related logic. It works with SPI library with MCP2515 chip.
*	
*
*/ 

#ifndef __CANBUS__H
#define __CANBUS__H


/**
* Key-codes for keys on steering wheel
*/
#define CAN_KEY_UP 1
#define CAN_KEY_DOWN 2
#define CAN_KEY_LEFT 3
#define CAN_KEY_RIGHT 4

#define CAN_KEY_ENTER 5
#define CAN_KEY_BACK 6



/**
* Initializes CANBUS communciation.
* @return 1 on success, an error-code on error.
*/
int canbus_init();

/**
* Reads pressed key from steering wheel.
* @return keycode on success, 0 means no key has been pressed
*/
int canbus_read_key();

/*
* @return 1 on successful initialization of CAN controller

*/
uint8_t canbus_status();

#endif