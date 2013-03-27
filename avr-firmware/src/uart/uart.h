#ifndef __UART__H
#define __UART__H

#include <stdio.h>

extern FILE uart0_output;
extern FILE uart0_input;

extern int uart_putchar(char c, FILE* stream);
extern void uart_putstring(char* c);
extern int uart_getchar(FILE* stream);

extern void uart_init(void);

/* http://www.ermicro.com/blog/?p=325 */

#endif
