/*
 * seriallcd.c
 *
 * Created: 12/26/2011 9:10:19 PM
 *  Author: Owner
 */ 

#import "global.h"
#import "seriallcd.h"
#import <avr/io.h>

#define BAUD 9600
#include <util/setbaud.h>

#define SERIAL_LCD_COMMAND				0xFE
#define SERIAL_LCD_BACKLIGHT_COMMAND	0x7C
#define SERIAL_LCD_CLEAR_SCREEN			0x01
#define SERIAL_LCD_LINE_ONE				0x80
#define SERIAL_LCD_LINE_TWO				0xC0
#define SERIAL_LCD_BACKLIGHT_ON			0x9D
#define SERIAL_LCD_BACKLIGHT_OFF		0x80
#define SERIAL_LCD_CURSOR_RIGHT			0x14
#define SERIAL_LCD_CURSOR_LEFT			0x10
#define SERIAL_LCD_SCROLL_RIGHT			0x1C
#define SERIAL_LCD_SCROLL_LEFT			0x18
#define SERIAL_LCD_VISUAL_ON			0x0C
#define SERIAL_LCD_VISUAL_OFF			0x08
#define SERIAL_LCD_UNDERLINE_CURSOR_ON	0x0E
#define SERIAL_LCD_UNDERLINE_CURSOR_OFF	0x0C
#define SERIAL_LCD_BOX_CURSOR_ON		0x0D	
#define SERIAL_LCD_BOX_CURSOR_OFF		0x0C


/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/

void _usart_transmit_byte(unsigned char data);

void serial_lcd_init()
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0B = (1<<RXCIE0) | (1<<TXCIE0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void serial_lcd_clear_screen(void)
{
	_usart_transmit_byte(SERIAL_LCD_COMMAND);
	_usart_transmit_byte(SERIAL_LCD_CLEAR_SCREEN);
}

void serial_lcd_set_line( u08 line)
{
	if( line<1 || line>2 )
		return;
	_usart_transmit_byte(SERIAL_LCD_COMMAND);
	_usart_transmit_byte( (line==1)?SERIAL_LCD_LINE_ONE:SERIAL_LCD_LINE_TWO);
}

void serial_lcd_set_backlight( BOOL isON)
{
	_usart_transmit_byte(SERIAL_LCD_BACKLIGHT_COMMAND);
	_usart_transmit_byte((isON)?SERIAL_LCD_BACKLIGHT_ON:SERIAL_LCD_BACKLIGHT_OFF);
}

void serial_lcd_set_position( u08 row, u08 col)
{
	_usart_transmit_byte(SERIAL_LCD_COMMAND);
	_usart_transmit_byte(col + row * 64 + 0x80);
}

void serial_lcd_set_cursor_type_state(CursorType type, BOOL state)
{
	_usart_transmit_byte(SERIAL_LCD_BACKLIGHT_COMMAND);
	if( !state )
		_usart_transmit_byte(SERIAL_LCD_BOX_CURSOR_OFF);
	else
		if( type == CursorTypeBox )
			_usart_transmit_byte(SERIAL_LCD_BOX_CURSOR_ON);
		else
			_usart_transmit_byte(SERIAL_LCD_UNDERLINE_CURSOR_ON);
}

void serial_lcd_write_string( char *str )
{
	u08 index = 0;
	while( str[index] )
	{
		_usart_transmit_byte( str[index++] );
	}
}

void serial_lcd_write_byte( char data )
{
	_usart_transmit_byte(data);
}

void _usart_transmit_byte(unsigned char data)
{
	while( !(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}