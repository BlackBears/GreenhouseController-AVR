/*
 * usart_am168.c
 *
 * Created: 1/3/2012 5:31:02 AM
 *  Author: Owner
 */ 
#include "global.h"
#include "uart-am168.h"
#include <avr/io.h>

void uartInit(u32 baudrate) {
	u16 baudval = ( F_CPU / baudrate / 16 ) - 1;
	UBRR0L = (u08)baudval;
	UBRR0H = (u08)(baudval >> 8);
	
	UCSR0C = (3 << UCSZ00);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void uartSendByte(u08 data) {
	while ( !(UCSR0A & (1 << UDRE0)) )
		;
	UDR0 = data;
}	

void uartSendStr(char *s) {
	while( *s )
		uartSendByte(*s++);
}
