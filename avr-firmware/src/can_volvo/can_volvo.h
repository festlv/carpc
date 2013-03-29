#ifndef __CAN_VOLVO__H
#define __CAN_VOLVO__H

typedef enum BUTTON_CODE {
	KEY_UP = 1,
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
extern BUTTON_CODE can_volvo_read_key();

#endif 