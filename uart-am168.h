/*
 * uart_am168.h
 *
 * Created: 1/3/2012 5:33:40 AM
 *  Author: Owner
 */ 


#ifndef UART_AM168_H_
#define UART_AM168_H_

#include "global.h"

void uartInit(u32 baudrate);
void uartSendByte(u08 data);
void uartSendStr(char *s);

#endif /* UART-AM168_H_ */