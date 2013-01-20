#ifndef __UART__H
#define __UART__H


extern void uart_putchar(char c);
extern void uart_putstring(char* c);
extern char uart_getchar();

extern void uart_init(void);

/* http://www.ermicro.com/blog/?p=325 */

#endif
