#include <util/delay.h>
#include <avr/io.h>

#include "config.h"
#include "spi/spi.h"
#include <stdio.h>
#include "canbus/canbus.h"
#include "canbus/mcp2515.h"


/** Initializes SPI in master mode, at CLK/16 = 1MHz.
*/
static void canbus_init_spi() {
	setup_spi(SPI_MODE_0, SPI_MSB, 0, SPI_MSTR_CLK4);
}

static inline void canbus_ss_low() {
	CANBUS_CS_PORT &= ~(1<<CANBUS_CS_BIT);
	_delay_ms(1);
}

static inline void canbus_ss_high() {
	CANBUS_CS_PORT |= (1<<CANBUS_CS_BIT);
	_delay_ms(1);
}
static inline uint8_t canbus_read_reg(uint8_t reg) {
	uint8_t tmp;
	canbus_ss_low();
	send_spi(READ);
	send_spi(reg);
	tmp=send_spi(0xff);
	canbus_ss_high();
	return tmp;
}
uint8_t canbus_status() {
	uint8_t tmp;
	canbus_ss_low();
	send_spi(READ_STATUS);
	tmp=send_spi(0xFF);
	canbus_ss_high();
	return tmp;
}


//reads contents of CANSTAT register
uint8_t canbus_status_reg() {
    return canbus_read_reg(CANSTAT);
}


//write contents of register
static inline void canbus_write_reg(uint8_t reg, uint8_t val) {
    canbus_ss_low();
	send_spi(WRITE);
	send_spi(reg);
	send_spi(val);
	canbus_ss_high();
}

uint8_t canbus_set_mode(uint8_t mode) {
	uint8_t mask = 0xE0;
	canbus_ss_low();
	send_spi(BIT_MODIFY);
	send_spi(CANCTRL);
	send_spi(mask);
	send_spi(mode);
	canbus_ss_high();
	uint8_t status = canbus_status_reg();
    if (status & mode)
        return 1;
    else return 0;
}

static void canbus_set_baudrate() {
    //sets baud rate to 125kbit/s
    canbus_ss_low();
	send_spi(WRITE);
	send_spi(CNF0);
	send_spi(0x03);
	canbus_ss_high();

	canbus_ss_low();
	send_spi(WRITE);
	send_spi(CNF1);
	send_spi(0xB8);
	canbus_ss_high();

	canbus_ss_low();
	send_spi(WRITE);
	send_spi(CNF2);
	send_spi(0x05);
	canbus_ss_high();
}



/**
* Resets MCP2515 by holding reset line low for 10ms
*/
void canbus_reset() {
	CANBUS_RESET_PORT &= ~(1<<CANBUS_RESET_BIT);
	_delay_ms(10);
	CANBUS_RESET_PORT |= (1<<CANBUS_RESET_BIT);
	_delay_ms(10);
}

static void canbus_init_pins() {
	CANBUS_RESET_DDR |= (1<<CANBUS_RESET_BIT);
	CANBUS_CS_DDR |= (1<<CANBUS_CS_BIT);
}
int8_t canbus_init_controller() {
    canbus_init_spi();
    canbus_init_pins();
    canbus_reset();
    canbus_set_baudrate();
    return 1;
}
int canbus_init() {
    canbus_init_controller();
    //sets CAN controller in listen mode
    if (canbus_set_mode(LOOPBACK)!=1) {
        printf("CAN init failed.\n");
        return 0;
    } else {
        return 1;
    }
}


uint8_t canbus_read_rx_buffer(uint8_t buffer_num, CAN_MESSAGE * msg) {
	char id_h, id_l, ed_h, ed_l;
	char cmd = buffer_num ? READ_RX_BUF_1_ID : READ_RX_BUF_0_ID;

	canbus_ss_low();
	send_spi(cmd);
	id_h = send_spi(0xFF);
	id_l = send_spi(0xFF);
	ed_h = send_spi(0xFF);
	ed_l = send_spi(0xFF); 
	//we're only interested in lowest 4 bits

    msg->data_len = send_spi(0xFF) & 0x0f;
     
	if (msg->data_len > 0) {

		for (uint8_t i = 0; i < 8;i++) {
			msg->data[i] = send_spi(0xFF);
		}
		//we could be done now, you know,
		//but we need to have some fun with byte mangling
		//see page 28 of MCP2515 datasheet
		if (id_l & 0x08) {
			//this is an extended frame, some more byte mangling
            msg->id = ((uint8_t) ed_l) | (((uint16_t) ed_h) << 8) | 
                (((uint32_t)id_l &0b11) << 16) | (((uint32_t)id_l&0b11100000)<<13) |
                (((uint32_t)id_h)<<21);
            //mask out upper three bits (standard + ext identifiers are only 29
            //bits long
            msg->id &= 0x1FFFFFFF;
            msg->ext = 1;
		} else {

		    msg->id = (uint16_t) (id_h << 3) | ((id_l&0b11100000) >> 5);
            msg->ext = 0;
        }
	}
	canbus_ss_high();
	return msg->data_len;
}
uint8_t canbus_write_tx_buffer(uint8_t buffer_num, CAN_MESSAGE *msg) {
	char send_cmd, tx_buf_reg;
	switch (buffer_num) {
		case 0:
			send_cmd = SEND_TX_BUF_0;
			tx_buf_reg = LOAD_TX_BUF_0_ID;
			break;
		case 1:
			send_cmd = SEND_TX_BUF_1;
			tx_buf_reg = LOAD_TX_BUF_1_ID;
			break;
		case 2:
			send_cmd = SEND_TX_BUF_2;
			tx_buf_reg = LOAD_TX_BUF_2_ID;
			break;
		default:
			send_cmd = SEND_TX_BUF_0;
			tx_buf_reg = LOAD_TX_BUF_0_ID;
	}

	canbus_ss_low();
	send_spi(tx_buf_reg);
	if (!msg->ext) {
		send_spi((uint8_t)(msg->id >> 3));
		send_spi((uint8_t)(msg->id<<5));
		send_spi(0x00);
		send_spi(0x00);
	} else {
		send_spi((uint8_t)(msg->id >> 21));
		send_spi((uint8_t) (((msg->id>>13) & 0xe0) | 0x08 | ((msg->id & 0x30000) >> 16)));
		send_spi(((uint16_t) msg->id) >> 8);
		send_spi((uint8_t) msg->id);
	}

	send_spi(msg->data_len);
	for (uint8_t i=0; i < msg->data_len; i++) {
		send_spi(msg->data[i]);
	}
	canbus_ss_high();

	canbus_ss_low();
	send_spi(send_cmd);
	canbus_ss_high();
    
	return msg->data_len;
}


uint8_t canbus_rx_status(uint8_t buffer_num) {
	uint8_t tmp;
    uint8_t mask = buffer_num ? 0x80 : 0x40;
    
	canbus_ss_low();
	send_spi(RX_STATUS);
	tmp=send_spi(0xFF);
	canbus_ss_high();
	return (tmp & mask) == mask;
}
static uint8_t canbus_write_mask(uint8_t mask_num, uint32_t mask, uint8_t extended) {
    uint8_t mask_reg;
    if (mask_num == 0) 
        mask_reg = MASK_0;
    else if (mask_num == 1)
        mask_reg = MASK_1;
    else
        return 2;

   if (extended) {
       canbus_write_reg(mask_reg, (uint8_t) (mask >> 21));
       canbus_write_reg(mask_reg+1, (uint8_t)(((mask >> 13) & 0b11100000) |
                    ((mask>>15) & 0b11)));
       canbus_write_reg(mask_reg+2, (uint8_t)(mask >> 8));
       canbus_write_reg(mask_reg+3, (uint8_t)mask);
   } else {
        canbus_write_reg(mask_reg, (uint8_t) (mask>>3));
        canbus_write_reg(mask_reg+1, (uint8_t) ((mask<<5)&0b11100000));
        canbus_write_reg(mask_reg+2, 0x00);
        canbus_write_reg(mask_reg+3, 0x00);
   }
   return 1;
}

static uint8_t canbus_write_filter(uint8_t filter_num, uint32_t filter, uint8_t extended) {
    char filter_reg;
    switch (filter_num) {
        case 0:
            filter_reg = FILTER_0;
            break;
        case 1:
            filter_reg = FILTER_1;
            break;
        case 2:
            filter_reg = FILTER_2;
            break;
        case 3:
            filter_reg = FILTER_3;
            break;
        case 4:
            filter_reg = FILTER_4;
            break;
        case 5:
            filter_reg = FILTER_5;
            break;
        default:
            return 1;
    }
    if (extended) {
        canbus_write_reg(filter_reg, (uint8_t) (filter >> 21));
        canbus_write_reg(filter_reg+1, (uint8_t)(((filter >> 13) & 0b11100000)
                    | ((filter>>15) & 0b11)
                    | 0b0001000));
        canbus_write_reg(filter_reg+2, (uint8_t)(filter >> 8));
        canbus_write_reg(filter_reg+3, (uint8_t)filter);
    } else {
        canbus_write_reg(filter_reg, (uint8_t) (filter>>3));
        canbus_write_reg(filter_reg+1, (uint8_t) ((filter<<5)&0b11100000));
        canbus_write_reg(filter_reg+2, 0x00);
        canbus_write_reg(filter_reg+3, 0x00);
   }
   return 1;

}
int8_t canbus_setup_filter(uint8_t filter_num, uint8_t extended,
    uint32_t mask, uint32_t filter) {
    if (filter_num>5)
        return 2;

    canbus_write_mask((filter_num < 2) ? 0: 1, mask, extended);
    canbus_write_filter(filter_num, filter, extended); 
    return 1;
}


