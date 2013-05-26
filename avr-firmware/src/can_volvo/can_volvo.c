#include "config.h"
#include <util/delay.h>
#include "can_volvo/can_volvo.h"
#include "canbus/canbus.h"
#include <stdio.h>


const uint32_t button_message_id = 0x040066;
const uint32_t heartbeat_message_id = 0x140044A;


uint8_t keymap[NUM_BUTTONS][BUTTON_DATA_LEN] = {
    {0x40, 0x00, 0xF5, 0x01, 0x84, 0xF4, 0x07},//Up
    {0xC0, 0x00, 0xF3, 0x01, 0x44, 0xF4, 0x07},//Down
    {0x80, 0x00, 0xF6, 0x01, 0x24, 0xF4, 0x07},//Left
    {0x80, 0x00, 0xF2, 0x01, 0x14, 0xF4, 0x07},//Right
    {0xC0, 0x00, 0xF3, 0x01, 0x04, 0xF6, 0x07},//Enter
    {0x80, 0x00, 0xF6, 0x01, 0x04, 0xF5, 0x07},//Back
    {0x80, 0x00, 0xF6, 0x01, 0x0f, 0xf4, 0x06},//Yes
    {0xc0, 0x00, 0xF3, 0x01, 0x04, 0xF4, 0x05},//no
    {0xC0, 0x00, 0xF3, 0x01, 0x04, 0x74, 0x07},//vol up
    {0x80, 0x00, 0xF2, 0x01, 0x0f, 0xB4, 0x07},//vol down
    {0x40, 0x00, 0xF1, 0x01, 0x04, 0xD4, 0x07},//next
    {0x80, 0x00, 0xF2, 0x01, 0x04, 0xE4, 0x07},//prev
};



static CAN_MESSAGE can_received_message;
static inline void relay_on() {
    RELAY_PORT |= (1<<RELAY_BIT);
}

static inline void relay_off() {
    RELAY_PORT &= ~(1<<RELAY_BIT);
}

void can_volvo_init() {
    canbus_init_controller();
    canbus_setup_filter(0, 1, 0xFFFFFFFF, button_message_id);
    canbus_setup_filter(2, 1, 0xFFFFFFFF, heartbeat_message_id);
    canbus_set_mode(LOOPBACK);
    RELAY_DDR |= 1<<RELAY_BIT;
    relay_on();
}
static void can_button_pressed(uint8_t button_code) {
    printf("Button press: %u\n", button_code+30);
}

static void can_parse_message(CAN_MESSAGE * msg) {
    int8_t button_num = -1;
    if (msg->id == button_message_id) {
        //naive comparison if data matches any of the buttons
        for (uint8_t btn_idx=0; btn_idx < NUM_BUTTONS; btn_idx++) {
            button_num = -1;
            for (uint8_t i=0; i < MIN(BUTTON_DATA_LEN, msg->data_len); i++) {
                if (msg->data[i] != keymap[btn_idx][i]) {
                    button_num = -2;
                    break;
                }
            }
            
            if (button_num == -1) {
                //button was found
                button_num = btn_idx;
                break;
            }
        }
        if (button_num>=0) {
            can_button_pressed(button_num);     
        }
    }
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
}



