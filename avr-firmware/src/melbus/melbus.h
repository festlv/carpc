#ifndef __MELBUS__H
#define __MELBUS__H

#include <stdint.h>

//define registers for CLK line
#define MELBUS_CLK_PORT         PORTD
#define MELBUS_CLK_PN           PD2
#define MELBUS_CLK_DDR          DDRD
#define MELBUS_CLK_PIN          PIND

//define registers for pin change interrupt on clock signal
#define MELBUS_CLK_PCIE         PCIE2
#define MELBUS_CLK_PCMSK_REG    PCMSK2
#define MELBUS_CLK_PCINT        PCINT18
#define MELBUS_CLK_PCINT_vect   PCINT2_vect
//registers for busy signal
#define MELBUS_BUSY_PORT        PORTD
#define MELBUS_BUSY_PN          PD7
#define MELBUS_BUSY_DDR         DDRD

//registers for data signal
#define MELBUS_DATA_PORT        PORTD
#define MELBUS_DATA_PN          PD4
#define MELBUS_DATA_DDR         DDRD
#define MELBUS_DATA_PIN         PIND

#define HU_COMMAND_COUNT        11

#define IN_BUFFER_LEN           6
//possible commands from HU
typedef enum HU_Command {
    HU_WAIT = 0,
    HU_INIT,
    HU_PLAY,
    HU_CART_INFO,
    HU_POWER_UP,
    HU_POWER_DOWN,
    HU_NEXT,
    HU_PREV,
    HU_FF,
    HU_FR,
    HU_SCAN,
    HU_RND,
    HU_VOL_UP,
    HU_VOL_DOWN,
} HU_Command;

typedef struct {
	volatile uint8_t length;
	volatile HU_Command cmd_code;
	volatile uint8_t cmd_data[6];
} Melbus_Command;

typedef struct {
	volatile uint8_t length;
	volatile uint8_t res_data[9];
} Melbus_Response;

typedef enum Melbus_State {
    STATE_RECEIVE = 0,
    STATE_SEND = 1
} Melbus_State;

typedef enum Melbus_Power_State {
    STATE_STOP = 0,
    STATE_PLAY = 1
} Melbus_Power_State;

//initializes pins, and interrupts
extern void melbus_init();

//notifies HU that there is a device on bus
extern void melbus_signal_presence();

//executes commands received from MELBUS
extern void melbus_step();

#endif
