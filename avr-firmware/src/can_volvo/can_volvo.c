#include "config.h"
#include <util/delay.h>
#include "can_volvo/can_volvo.h"
#include "canbus/canbus.h"

#include "time/time.h"
#include <stdio.h>
#include <string.h>

uint32_t button_seen_time[NUM_BUTTONS];

const uint32_t button_message_id = 0x00400066;//0x00400066;//0x660400;

//Keypress events are sent as canbus messages in the following format (xx and
//yy are single byte values):
//xx 00 00 xx 1f 40 yy yy
//xx changes for every keypress- they are incremented at each keypress
//yy bytes are fixed- we only need to check if those match
//in fact- we only need to check last 2 bytes of incoming message

uint8_t keymap[NUM_BUTTONS][BUTTON_DATA_LEN] = {
    {0x48, 0x7f},//Up
    {0x44, 0x7f},//Down
    {0x42, 0x7f},//Left
    {0x41, 0x7f},//Right
    {0x60, 0x7f},//Enter
    {0x50, 0x7f},//Back
    {0x40, 0x6f},//Yes
    {0x40, 0x5f},//No
    {0x40, 0x77},//Vol up
    {0x40, 0x7b},//Vol down
    {0x40, 0x7d},//Next
    {0x40, 0x7e},//Prev
};


//button messages are sent ~ 13 times/second (while the button is pressed)
//so, if we receive a button press- we send a message about keypress event
//after 1/10s, if we don't receive it again in 100ms- send a keyreleased event
const uint8_t BUTTON_TIMEOUT = 100;


static CAN_MESSAGE can_received_message;
static inline void relay_on() {
    RELAY_PORT |= (1<<RELAY_BIT);
}

static inline void relay_off() {
    RELAY_PORT &= ~(1<<RELAY_BIT);
}

void can_volvo_init() {
    canbus_init_controller();

    #ifndef CANBUS_LOGGING_MODE
        canbus_setup_filter(0, 1, 0xFFFFFFFF, button_message_id);
    #endif 

    canbus_set_mode(LISTEN);
    RELAY_DDR |= 1<<RELAY_BIT;
    relay_on();
}
static void can_button_pressed(uint8_t button_code) {
    if (button_seen_time[button_code] == 0) {
        printf("%u:1\n", button_code);
    }
    button_seen_time[button_code] = time_ms();
}
static void can_button_released(uint8_t button_code) {
    printf("%u:2\n", button_code);
    button_seen_time[button_code] = 0;
}
static void can_parse_message(CAN_MESSAGE * msg) {
    if (msg->id == button_message_id) {
        //naive comparison if data matches any of the buttons
        
        for (uint8_t btn_idx=0; btn_idx < NUM_BUTTONS; btn_idx++) {
            char * tmp = (char*)msg->data + 6;
            if (memcmp(tmp, keymap[btn_idx], BUTTON_DATA_LEN)==0)
                can_button_pressed(btn_idx);
        }
    } 
    #ifdef CANBUS_LOGGING_MODE 
        printf("%08lx:", msg->id);
        for (int8_t i=0;i<msg->data_len ;i++)
            printf("%02x ", msg->data[i]);
        printf("\n");
    #endif
}

void can_volvo_step() {
    int8_t buf_num=-1;
    
    if (canbus_rx_status(0)) {
        buf_num = 0; 
    } else if (canbus_rx_status(1)) {
        buf_num = 1;
    }

    if (buf_num!=-1 && canbus_read_rx_buffer(buf_num, &can_received_message) >0 ) {
        can_parse_message(&can_received_message);
    }

    //process key release events
    uint32_t now= time_ms();
    for (uint8_t i=0; i< NUM_BUTTONS; i++) {
        if (button_seen_time[i] != 0 && 
                (button_seen_time[i] + BUTTON_TIMEOUT< now)) {
            can_button_released(i);
        }            
    }
}



