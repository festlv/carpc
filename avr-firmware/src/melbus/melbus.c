#include "avr/interrupt.h"
#include "util/delay.h"

#include "melbus/melbus.h"
#include "uart/uart.h"


static const uint8_t DEV_ID = 0xe8;

static Melbus_Command commands[HU_COMMAND_COUNT] = {
								{3, HU_INIT, {0x07, 0x1A, 0xEE}},
								{2, HU_CART_INFO, {0x1E, 0xEF}},
								{4, HU_PLAY, {0x1B, 0xE0, 0x01, 0x08}},
								{2, HU_POWER_UP, {0x19, 0x2F}},
								{2, HU_POWER_DOWN, {0x19, 0x22}},
								{4, HU_NEXT, {0x1B,0x2D,0x40,0x01}},
								{4, HU_PREV, {0x1B,0x2D,0x00,0x01}},
								{2, HU_FF, {0x19, 0x29}},  // FF button pressed, when released HU sends powerup command
								{2, HU_FR, {0x19, 0x26}},   // FR button pressed, when released HU sends powerup command
								{2, HU_SCAN, {0x19, 0x2E}},
								{2, HU_RND, {0x19, 0x52}}
};

static Melbus_Response cart_info = {6, {0x00, 0x0F, 0xFF, 0x4A, 0xFC, 0xFF}};
static Melbus_Response play_info = {9, {0x00, 0x02, 0x00, 0x01, 0x80, 0x01, 0xFF, 0x06, 0x06}};

static volatile Melbus_State bus_state = STATE_RECEIVE;


static volatile Melbus_Power_State power_state = STATE_PLAY;

static volatile HU_Command cmd_found = HU_WAIT;
static volatile HU_Command current_command = HU_WAIT;

static uint8_t in_buffer[IN_BUFFER_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static volatile uint8_t data_out = 0; 
static volatile uint8_t sent = 0;
static volatile uint8_t byte_out = 0;


//variables for interrupt
static volatile uint8_t readClock = 7, sendClock = 7;
static volatile uint8_t readByte = 0, byteCopy = 0;


static void put_in_buffer(uint8_t byte);

static void melbus_init_interrupt() {
    //enable corresponding pin change interrupt
    PCICR |= (1 << MELBUS_CLK_PCIE);
    //enable the correct PCINT in pin change interrupt mask register
    MELBUS_CLK_PCMSK_REG |= (1 << MELBUS_CLK_PCINT);
}


volatile uint8_t latch_data_val;

//pin change interrupt on the clock line
//reads incoming data on rising edge
//writes outgoing data on falling edge
ISR(MELBUS_CLK_PCINT_vect) {
    latch_data_val = MELBUS_DATA_PIN;

    if (MELBUS_CLK_PIN & (1 << MELBUS_CLK_PN)) {
        if (bus_state == STATE_RECEIVE) {
            
            if (latch_data_val & (1 << MELBUS_DATA_PN)) {
                readByte |= (1 << readClock);
            } else {
                readByte &= ~(1 << readClock);
            }

            if (readClock == 0) {
                // Once 1 byte is received put into serial and into buffer
                put_in_buffer(readByte);
                uart_putchar(readByte, NULL);
                readClock = 7;
            } else {
                readClock--;
            }
        }

    } else {
        //TODO: if any other bit is added to pin change mask, we must check if
        //PD2 is really the source of interrupt
        
        if (bus_state == STATE_SEND) {
            MELBUS_DATA_DDR |= (1 << MELBUS_DATA_PN);

            if (data_out & (1 << sendClock)) {
                MELBUS_DATA_PORT |= (1 << MELBUS_DATA_PN);
            } else {
                MELBUS_DATA_PORT &= ~(1 << MELBUS_DATA_PN);
            }
            
            //write to trigger output
            PORTC |= (1 << PC5);


            if (sendClock == 0) {
                sendClock = 7;
                bus_state = STATE_RECEIVE;
                put_in_buffer(data_out);
                uart_putchar(data_out, NULL);
                MELBUS_DATA_DDR &= ~(1 << MELBUS_DATA_PN);
                MELBUS_DATA_PORT |= (1 << MELBUS_DATA_PN);
            } else {
                sendClock--;
            }
        }        
    }

}
void melbus_init() {

    //initialize clock as input, enable pullup
    MELBUS_CLK_DDR &= ~(1 << MELBUS_CLK_PN);
    MELBUS_CLK_PORT |= (1 << MELBUS_CLK_PN);

    //initialize data as input
    MELBUS_DATA_DDR &= ~(1 << MELBUS_DATA_PN);
    MELBUS_DATA_PORT |= (1 << MELBUS_DATA_PN);

    //initialize busy pin as input- it will be changed to output when needed
    MELBUS_BUSY_DDR &= ~(1 << MELBUS_BUSY_PN);
    
    DDRC |= (1 << PC5);
    PORTC &= ~(1 << PC5);
    
    melbus_init_interrupt();
    sei();
    melbus_signal_presence();
}


/** 
 * INTERNAL PROTOCOL IMPLEMENTATION GOES BELOW
*/

static void respond_to_cmd() {
	switch (cmd_found) {
	case (HU_WAIT):
			break;

	case (HU_INIT):
            //respond with device id when it's required
			if (in_buffer[5] == DEV_ID) {
				bus_state = STATE_SEND;
				data_out = DEV_ID;
				cmd_found = HU_WAIT;
			}
			break;

	case (HU_CART_INFO):
			bus_state = STATE_SEND;
			data_out = cart_info.res_data[byte_out];
			byte_out++;
			if (byte_out >= cart_info.length) {
				cmd_found = HU_WAIT;
				byte_out = 0;
			}
			break;

	case (HU_PLAY):
			bus_state = STATE_SEND;
			data_out = play_info.res_data[byte_out];
			byte_out++;
			if (byte_out >= play_info.length) {
				cmd_found = HU_WAIT;
				byte_out = 0;
			}

			break;

	case (HU_POWER_UP):
			bus_state = STATE_SEND;
			data_out = 0x00;
			if (power_state == STATE_STOP) {
				play_info.res_data[1] = 0x08;
				power_state = STATE_PLAY;
				//cmd_found = HU_WAIT;
			} else {
				// Process FR command (Bluetooth vol_dn)
			}
			cmd_found = HU_WAIT;

			break;

	case (HU_POWER_DOWN):
			bus_state = STATE_SEND;
			data_out = 0x00;
			play_info.res_data[1] = 0x02;
			power_state = STATE_STOP;
			cmd_found = HU_WAIT;

			break;

	case (HU_NEXT):
			play_info.res_data[5]++;
			if (play_info.res_data[5] > 0x99){
				play_info.res_data[5] = 0x01;
			}
			if ((play_info.res_data[5] & 0x0F) > 0x09){
				play_info.res_data[5] += 6;
			}
			current_command = HU_NEXT;
			cmd_found = HU_WAIT;

			break;

	case (HU_PREV):
			play_info.res_data[5]--;

			if (play_info.res_data[5] < 0x01)
				play_info.res_data[5] = 0x99;

			if ((play_info.res_data[5] & 0x0F) == 0x0F)
				play_info.res_data[5] -= 6;

			current_command = HU_PREV;
			cmd_found = HU_WAIT;

			break;

	case (HU_FF):
			current_command = HU_VOL_UP;
			cmd_found = HU_WAIT;
			break;

	case (HU_FR):
			current_command = HU_VOL_DOWN;
			cmd_found = HU_WAIT;
			break;

	case (HU_SCAN):
			current_command = HU_SCAN;
			cmd_found = HU_WAIT;
			break;

	case (HU_RND):
			current_command = HU_PLAY;
			cmd_found = HU_WAIT;
			break;
	default:
			cmd_found = HU_WAIT;
			break;
	}
}

static void find_commands(volatile uint8_t *buff, Melbus_Command *cmds, int cmdLen) {
	// Get buffer, command and their corresponding lengths
	//int startingPos = bLen - cLen 6 - 3 = 3
	char found = 1;
	for (int command = 0; command < cmdLen; command++) {
		int startingPos = IN_BUFFER_LEN - cmds[command].length;
		int count = 0;
		for (int i = 0; i < cmds[command].length; i++) {
			if (buff[startingPos + i] == cmds[command].cmd_data[i]) {
				count++;
				if (count == cmds[command].length) {
					found = 1;
					cmd_found = cmds[command].cmd_code;
					break;
				}
			} else {
				found = 0;
				break;
			}
		}
		if (found)
			break;
	}
	if (found) {
		respond_to_cmd();
	} else {
		cmd_found = HU_WAIT;
	}
}

static void put_in_buffer(uint8_t byte){
	in_buffer[0] = in_buffer[1];
	in_buffer[1] = in_buffer[2];
	in_buffer[2] = in_buffer[3];
	in_buffer[3] = in_buffer[4];
	in_buffer[4] = in_buffer[5];
	in_buffer[5] = byte;
	if (cmd_found)
		respond_to_cmd();
	else
		find_commands(in_buffer, commands, HU_COMMAND_COUNT);
}

//notifies HU that there is a device on bus
void melbus_signal_presence() {
    MELBUS_BUSY_DDR |= (1 << MELBUS_BUSY_PN);
    MELBUS_BUSY_PORT &= ~(1 << MELBUS_BUSY_PN);

    _delay_ms(1000);

    MELBUS_BUSY_PORT |= (1 << MELBUS_BUSY_PN);
    MELBUS_BUSY_DDR &= ~(1 << MELBUS_BUSY_PN);
    printf("#Presence signalled\n");
}


void melbus_step() {
    switch (current_command) {
        case HU_PLAY:
            uart_putchar('H', NULL);
        break;

        case HU_WAIT:
        case HU_INIT:
        case HU_CART_INFO:
        case HU_POWER_UP:
        case HU_POWER_DOWN:
        case HU_NEXT:
        case HU_PREV:
        case HU_FF:
        case HU_FR:
        case HU_SCAN:
        case HU_RND:
        case HU_VOL_UP:
        case HU_VOL_DOWN:
        default:

        break;
    }
}
