#include "config.h"
#include <util/delay.h>
#include "can_volvo/can_volvo.h"
#include "canbus/canbus.h"
#include <stdio.h>



const uint32_t button_message_id = 0x00400066;//0x00400066;//0x660400;


uint8_t keymap[NUM_BUTTONS][BUTTON_DATA_LEN] = {
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x48, 0x7f},//Up
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x44, 0x7f},//Down
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x42, 0x7f},//Left
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x41, 0x7f},//Right
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x60, 0x7f},//Enter
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x50, 0x7f},//Back
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x40, 0x6f},//Yes
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x40, 0x5f},//No
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x40, 0x77},//Vol up
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x40, 0x7b},//Vol down
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x40, 0x7d},//Next
    {0x00, 0x00, 0x00, 0x1f, 0x40, 0x40, 0x7e},//Prev
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
    canbus_set_mode(LISTEN);
    RELAY_DDR |= 1<<RELAY_BIT;
    relay_on();
}
static void can_button_pressed(uint8_t button_code) {
    printf("%u:1\n", button_code);
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
    /*
    printf("%08lx:", msg->id);
    for (int8_t i=msg->data_len - 1;i>0;i--)
        printf("%02x", msg->data[i]);
    printf("\n");
    */

   
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



