/** 
* This file contains CANBUS-related logic. It works with SPI library with MCP2515 chip.
*	
*
*/ 

#ifndef __CANBUS__H
#define __CANBUS__H
#include "canbus/mcp2515.h"


typedef struct CAN_message {
	//message data
	uint8_t data[8];
	//message identifier
	uint32_t id;

	//length of data
	uint8_t data_len;
	// 1 if this message's id is in extended format (4 bytes)
	// 0 if this messages id is in normal format (2 bytes)
	uint8_t ext;
} CAN_MESSAGE;

/**
* Initializes CANBUS controller, places it in listen mode with default settings
*
* @return 1 on success, an error-code on error.
*/
extern int canbus_init();

/**
 * Initializes CANBUS controller, leaves it in configuration mode
 * @return 1 on success, an error code on error
 */
extern int8_t canbus_init_controller();

/**
 * Writes mask and filter values to appropriate CANBUS controller registers.
 * Filters 0 and 1 share the same mask, and filters 2, 3, 4, 5 share another
 * mask. See page 32 of MCP2515 datasheet for details.
 * @return 1 on success, an error code on error
 */
extern int8_t canbus_setup_filter(uint8_t filter_num, uint8_t extended,
    uint32_t mask, uint32_t filter);


/** 
 * Sets CANBUS controller into one of the following modes:
 * LOOPBACK
 * LISTEN
 * SLEEP
 * CONFIGURE
 *
 * @return 1 on success (verifies state of controller after issuing command),
 * 0 otherwise
 */

extern uint8_t canbus_set_mode(uint8_t mode);


/*
* @return Contents of CANSTAT register 

*/
extern uint8_t canbus_status_reg();


//reads receive buffer into CAN_MESSAGE struct pointed to by msg
//returns length of data received (same as msg->data_len)
//there are two read buffers- 0 and 1
extern uint8_t canbus_read_rx_buffer(uint8_t buffer_num, CAN_MESSAGE * msg);
extern uint8_t canbus_write_tx_buffer(uint8_t buffer_num, CAN_MESSAGE *msg);

//return 1 if there is message in buffer `buffer_num`  
extern uint8_t canbus_rx_status(uint8_t buffer_num);

#endif
