#ifndef __UART__H
#define __UART__H

#include <stdio.h>

extern void uart_putchar(char c, FILE *stream);
extern char uart_getchar(FILE *stream);

extern void uart_init(void);

/* http://www.ermicro.com/blog/?p=325 */

extern FILE uart_output;
extern FILE uart_input;

#endif