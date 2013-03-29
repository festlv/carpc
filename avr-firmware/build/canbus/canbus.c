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

uint8_t canbus_status() {
	uint8_t tmp;
	canbus_ss_low();
	send_spi(READ);
	send_spi(CANSTAT);
	tmp=send_spi(0x00);
	canbus_ss_high();
	return tmp;
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


int canbus_init() {
	canbus_init_spi();
	CANBUS_RESET_DDR |= (1<<CANBUS_RESET_BIT);
	CANBUS_CS_DDR |= (1<<CANBUS_CS_BIT);

	canbus_reset();
	printf("status: %x\n", canbus_status());
	//Sends appropriate CNF registers for 125kbit/s CAN baud rate

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

	//sets CAN controller in listen mode

	int mask = 0xE0;
	canbus_ss_low();
	send_spi(BIT_MODIFY);
	send_spi(CANCTRL);
	send_spi(mask);
	send_spi(LOOPBACK);
	canbus_ss_high();


	printf("status: %x\n", canbus_status());

	uint8_t status = canbus_status();
	uint8_t req_status = 0b01100000;

	if ((status & req_status) == req_status) {
		return 0;
	}
	else return 1;

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
		msg->id = (uint16_t) (id_h << 3) | ((id_l&0b11100000) >> 5);
        printf("ed_h: %hhx\n", ed_h);
        printf("ed_l: %hhx\n", ed_l);
		if (id_l & 0x08) {
			//this is an extended frame, some more byte mangling
			msg->extended_id = ((uint32_t) (id_l&0b11) << 16) | 
				(((uint32_t) ed_h) << 8) | 
				((uint32_t) ed_l&0xFF);
		}

		msg->ext = (id_l & 0x08)==0x08;
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
	}
	else {
		send_spi((uint8_t)(msg->id >> 3));
		send_spi( 
			(uint8_t) ((0xff & (msg->id<<5)) | 0x08 | (msg->extended_id >> 16)));
		send_spi((uint16_t) msg->extended_id >> 8);
        printf("ted_h: %hhx\n", (uint16_t) msg->extended_id >> 8);
        printf("ted_l: %hhx\n", (uint8_t) msg->extended_id);
		send_spi((uint8_t) msg->extended_id);
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
