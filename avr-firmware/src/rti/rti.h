#ifndef RTI_H
#define RTI_H
#include <stdint.h>
/*
Volvo RTI navigation (2002-2005?) control.
The screen waits for special commands via serial interface, 2400N1
Since it's connected to MCU via NPN transistor, the serial interface must be inversed (handled by SoftwareSerial).

Brief overview of protocol:
Screen expects three bytes (actually the serial protocol consists of 15/16 bit words and it's a lucky coincidence that it works by sending 8bit bytes).

The first byte is display mode.
The second byte is brightness level. If the brightness level needs to be changed, a special byte is sent. Otherwise, current brightness level is retained by sending 0x40.
The last byte is always 0x83 (don't ask).

For more information see: http://translate.googleusercontent.com/translate_c?depth=1&hl=en&prev=/search%3Fq%3Dwww.youtube.com/watch%253Fv%253DmQNQ2uDwl6I%26hl%3Den%26client%3Dubuntu%26channel%3Dcs%26biw%3D1916%26bih%3D971%26prmd%3Dimvns&rurl=translate.google.lv&sl=sv&twu=1&u=http://www.svxc.se/phpBB2/viewtopic.php%3Ft%3D18273%26postdays%3D0%26postorder%3Dasc%26start%3D180%26sid%3Dc0b78a6ad506d1f0017904843254b43e&usg=ALkJrhhEJZjEm8LNM9Lp9AkqJ3ZztT6RNQ
*/

extern void rti_init();

extern void rti_set_brightness(uint8_t level);

extern void rti_disable_screen();

extern void rti_enable_screen();
extern void rti_step();

#endif