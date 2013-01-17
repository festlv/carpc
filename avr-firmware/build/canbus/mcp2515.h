/**
* MCP2515 register addresses
*/
#ifndef __MCP2515__H
#define __MCP2515__H




#define LISTEN 0x60
#define LOOPBACK 0x40
#define CONFIGURATION 0x80
#define NORMAL 0x00
#define SLEEP 0x20




#define RESET_REG 0xc0  
#define READ 0x03 
#define WRITE 0x02 //read and write comands for SPI

#define READ_RX_BUF_0_ID 0x90
#define READ_RX_BUF_0_DATA 0x92
#define READ_RX_BUF_1_ID 0x94
#define READ_RX_BUF_1_DATA 0x96 //SPI commands for reading CAN RX buffers

#define LOAD_TX_BUF_0_ID 0x40
#define LOAD_TX_BUF_0_DATA 0x41
#define LOAD_TX_BUF_1_ID 0x42
#define LOAD_TX_BUF_1_DATA 0x43
#define LOAD_TX_BUF_2_ID 0x44
#define LOAD_TX_BUF_2_DATA 0x45 //SPI commands for loading CAN TX buffers

#define SEND_TX_BUF_0 0x81
#define SEND_TX_BUF_1 0x82
#define SEND_TX_BUF_2 0x83 //SPI commands for transmitting CAN TX buffers

#define READ_STATUS 0xA0
#define RX_STATUS 0xB0
#define BIT_MODIFY 0x05 //Other commands


//Registers
#define CNF0 0x2A
#define CNF1 0x29
#define CNF2 0x28
#define TXB0CTRL 0x30 
#define TXB1CTRL 0x40
#define TXB2CTRL 0x50 //TRANSMIT BUFFER CONTROL REGISTER
#define RXB0CTRL 0x60
#define RXB1CTRL 0x70
#define TXB0DLC 0x35 //Data length code registers
#define TXB1DLC 0x45
#define TXB2DLC 0x55
#define CANCTRL 0x0F //Mode control register
#define CANSTAT 0x0E //Mode status register
#define CANINTF 0x2C //Mode status register
#define CANINTE 0x2B //Mode status register
#define BFPCTRL 0x0c

#define MASK_0 0x20
#define MASK_1 0x24
#define FILTER_0 0x00
#define FILTER_1 0x04
#define FILTER_2 0x08
#define FILTER_3 0x10
#define FILTER_4 0x14
#define FILTER_5 0x18

#endif