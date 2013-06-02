#ifndef __CAN_VOLVO__H
#define __CAN_VOLVO__H

#include "stdint.h"

#define NUM_BUTTONS 12
#define BUTTON_DATA_LEN 2

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define BACK_BUTTON_POWEROFF_TIME 2000
#define NO_CAN_MESSAGE_POWEROFF_TIME 30000

typedef enum BUTTON_CODE {
	KEY_UP = 0,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_ENTER,
	KEY_BACK,
	KEY_YES,
	KEY_NO,
	KEY_VOL_UP,
	KEY_VOL_DOWN,
	KEY_NEXT,
	KEY_PREV
} BUTTON_CODE;

extern void can_volvo_init();
extern void can_volvo_step();



#endif 
