/** 
* This file contains CANBUS-related logic. It works with SPI library with MCP2515 chip.
*	
*
*/ 

#ifndef __CANBUS__H
#define __CANBUS__H

typedef struct CAN_message {
	//message data
	char data[8];
	//message identifier
	uint32_t id;

	//length of data
	uint8_t data_len;
	// 1 if this message's id is in extended format (4 bytes)
	// 0 if this messages id is in normal format (2 bytes)
	uint8_t ext;
} CAN_MESSAGE;

/**
* Initializes CANBUS communciation.
* @return 1 on success, an error-code on error.
*/
extern int canbus_init();

/*
* @return 1 on successful initialization of CAN controller

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
