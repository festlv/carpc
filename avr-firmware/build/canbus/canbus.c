#include <util/delay.h>
#include <avr/io.h>

#include "../config.h"
#include "../spi/spi.h"

#include "canbus.h"
#include "mcp2515.h"


/** Initializes SPI in master mode, at CLK/16 = 1MHz.
*/
int canbus_init_spi() {
	setup_spi(SPI_MODE_0, SPI_MSB, 0,SPI_MSTR_CLK4);
}

void canbus_ss_low() {
	PORTC |= (1<<PC0);
	_delay_ms(1);
}


void canbus_ss_high() {
	PORTC |= (1<<PC0);
	_delay_ms(1);
}
/**
* Resets MCP2515 by holding reset line low for 10ms
*/
void canbus_reset() {
	PORTC &= ~(1<<PC1);
	_delay_ms(10);
	PORTC |= (1<<PC1);
	_delay_ms(10);
}


int canbus_init() {
	canbus_init_spi();
	DDRC |= (1<<PC0) | (1<<PC1);

	canbus_reset();

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

	return 0;
}
int canbus_read_key() {
	return 0;
}