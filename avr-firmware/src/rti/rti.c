#include "rti/rti.h"
#include "config.h"
#include <util/delay.h>
#include "time/time.h"
#include <stdio.h>
//delay between bytes, ms
static const int rti_delay = 100; 

enum display_mode_name {RTI_RGB, RTI_PAL, RTI_NTSC, RTI_OFF};

static const char display_modes[] = {0x40, 0x45, 0x4C, 0x46};

static const char brightness_levels[] = {0x20, 0x61, 0x62, 0x23, 0x64, 0x25, 0x26, 0x67, 0x68, 0x29, 0x2A, 0x2C, 0x6B, 0x6D, 0x6E, 0x2F};

static int current_display_mode = RTI_PAL;
static uint8_t send_brightness = 1;
static uint8_t current_brightness_level = 5;

static uint8_t current_byte=0;

//delay between bits.
//it should actually be 416, but since interrupts take their time,
//the value needs to be lower
const int rti_bit_delay = 405;

static uint32_t rti_step_time=0;

void rti_bitbang_tx(char byte_to_tx);


void rti_init() {
	RTI_SERIAL_TX_DDR |= (1<<RTI_SERIAL_TX_BIT);
}

static inline void rti_tx_bit_delay() {
	_delay_us(rti_bit_delay);
}

void rti_step() {
	if (time_ms()>rti_step_time) {
		switch (current_byte) {
			case 0:
				rti_bitbang_tx(display_modes[current_display_mode]);
				current_byte++;
				break;
			case 1:
				if (send_brightness) {
					rti_bitbang_tx(brightness_levels[current_brightness_level]);
					send_brightness = 0;
				}
				else
					rti_bitbang_tx(0x40);
				current_byte++;
				break;
			case 2:
				rti_bitbang_tx(0x83);
				current_byte = 0;
				break;
		}
		rti_step_time = time_ms()+rti_delay;
	}
}

void rti_set_brightness(uint8_t level) {
	if (level>0 && level<16) {
		current_brightness_level = level;
		send_brightness = 1;
	}
}

void rti_disable_screen() {
	current_display_mode = RTI_OFF;
}

void rti_enable_screen() {
	current_display_mode = RTI_PAL;
}

//inverted logic serial bitbang transmit
void rti_bitbang_tx(char byte_to_tx) {
	RTI_SERIAL_TX_PORT |= (1<<RTI_SERIAL_TX_BIT);
	rti_tx_bit_delay();
	for (char mask= 0x01; mask; mask <<= 1)
    {
      if (byte_to_tx & mask) // choose bit
      {
      	RTI_SERIAL_TX_PORT &= ~(1<<RTI_SERIAL_TX_BIT);
      }
      else {
      	RTI_SERIAL_TX_PORT |= (1<<RTI_SERIAL_TX_BIT);
  	 	} 
      rti_tx_bit_delay();
    }
  	RTI_SERIAL_TX_PORT &= ~(1<<RTI_SERIAL_TX_BIT);
}
