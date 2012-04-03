
/*
 * greenhouse_controller.c
 *
 * Created: 12/23/2011 8:07:06 AM
 *  Author: Alan Duncan
 */ 

#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdio.h>
#include "time.h"
#include "mcp23017.h"
#include "mcp23008.h"
#include "lights.h"
#include "fans.h"
#include "seriallcd.h"
#include "keypad.h"
#include "command.h"
#include "uart-am168.h"
#include "./defs/command_states.h"			//	state machine constants
#include "./defs/commands.h"				//	keypad commands
#include "./defs/pindefs.h"					//	pin/port definitions not otherwise specified
#include "./peripherals/internal_fan.h"		
#include "./peripherals/heater.h"
#include "./peripherals/ds1307.h"
#include "./functionality/lcd.h"


/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void _init_light_specs();
void _init_pulse();
void _display_fan_pattern();
void _display_fan_time();
void _display_light_state();

/************************************************************************/
/* FUNCTION PROTOTYPES                                                  */
/************************************************************************/
void update_display();
void cls();
void lcd_set_line(u08 n);
void lcd_clear_print(char *s);
void lcd_clear_print_ready();
void lcd_OK_wait();
void lcd_cancel_wait();
void lcd_print_str_consecutively(char *s1, char *s2);

Time clock;
MCP23017 fan_controller;				//	MCP23017 fan driver
volatile LightSpecs light_specs;					//	specifications for our lights
char buffer[20];

volatile uint8_t time_updated;
volatile Keypad k; 
volatile MCP23008 dx;	//	indicator just for debugging
volatile MCP23008 lcd;
u08 data[2];
volatile u16 user_command_state;

u16 cmd_temp;
u08 keypad_char;

//	keypad and command conditions
#define KEYPAD_CHAR_PROCESSED 0xFE
#define KEYPAD_COMMAND_PROCESSED 0xFFFF

/************************************************************************/
/* PRECOMPILER DEFINES                                                  */
/************************************************************************/
#define ENABLE_KEYPAD_INTERRUPT EIMSK |= (1<<INT1)
#define DISABLE_KEYPAD_INTERRUPT EIMSK &= ~(1<<INT1)
#define DEBUG(a) mcp23008_write_register(&dx,MCP23008_GPIO,a)
#define LCD_PRINT_CONSECUTIVELY(a,b) cls(); \
	uartSendStr(a); \
	lcd_set_line(2); \
	uartSendStr(b);

/************************************************************************/
/* EEPROM/DEFAULTS                                                      */
/************************************************************************/
#define EEPROM_INIT_COMPLETE_CODE 0xAA
#define EEPROM_INIT_COMPLETE_ADDR 0x00
#define FAN_ON_SET_POINT_ADDR	0x01
#define FAN_OFF_SET_POINT_ADDR	0x02
//	EEPROM ADDRESSES
#define EEPROM_LIGHTS_ON_HOUR_ADDR			0x01
#define EEPROM_LIGHTS_ON_MINUTE_ADDR		0x02
#define EEPROM_LIGHTS_ON_SECOND_ADDR		0x03
#define EEPROM_LIGHTS_OFF_HOUR_ADDR			0x04
#define EEPROM_LIGHTS_OFF_MINUTE_ADDR		0x05
#define EEPROM_LIGHTS_OFF_SECOND_ADDR		0x06
#define EEPROM_FAN_MODE_ADDR				0x07
#define EEPROM_INTERNAL_FAN_ON_TEMP_ADDR	0x08
#define EEPROM_INTERNAL_FAN_OFF_TEMP_ADDR	0x09
#define EEPROM_HEATER_MODE_ADDR				0x0A
#define EEPROM_HEATER_ON_TEMP_ADDR			0x0B
#define EEPROM_HEATER_OFF_TEMP_ADDR			0x0C
#define EEPROM_HEATER_ON_HOUR_ADDR			0x0D
#define EEPROM_HEATER_ON_MINUTE_ADDR		0x0E
#define EEPROM_HEATER_OFF_HOUR_ADDR			0x0F
#define EEPROM_HEATER_OFF_MINUTE_ADDR		0x10

#define EEPROM_DEFAULT_LIGHTS_ON_HOUR		5		//	lights ON at 5:30:00
#define EEPROM_DEFAULT_LIGHTS_ON_MINUTE		30
#define EEPROM_DEFAULT_LIGHTS_ON_SECOND		00
#define EEPROM_DEFAULT_LIGHTS_OFF_HOUR		20		//	lights OFF at 20:30:00
#define EEPROM_DEFAULT_LIGHTS_OFF_MINUTE	00
#define EEPROM_DEFAULT_LIGHTS_OFF_SECOND	00
#define EEPROM_DEFAULT_FAN_MODE				01		//	mode B
#define EEPROM_DEFAULT_INT_FAN_ON_TEMP		27		//	27 degrees C
#define EEPROM_DEFAULT_INT_FAN_OFF_TEMP		25		//	25 degrees C
#define EEPROM_DEFAULT_HEATER_MODE			k_heater_mode_cyclic
#define EEPROM_DEFAULT_HEATER_ON_TEMP		25
#define EEPROM_DEFAULT_HEATER_OFF_TEMP		28
#define EEPROM_DEFAULT_HEATER_ON_HOUR		04
#define EEPROM_DEFAULT_HEATER_ON_MINUTE		15
#define EEPROM_DEFAULT_HEATER_OFF_HOUR		00
#define EEPROM_DEFAULT_HEATER_OFF_MINUTE	15

/*	The computed value is 34285; but empirical testing suggests that 33142 gives a more accurate result. */
static const uint16_t kTimerReloadConstant = 33142;

void eeprom_setup()
{
	u08 eeprom_did_init =  eeprom_read_byte(EEPROM_INIT_COMPLETE_ADDR);
	if( eeprom_did_init == EEPROM_INIT_COMPLETE_CODE ) {
		//	we previously initialized the defaults in EEPROM
		/*	first deal with lights times */
		lights_on_hour = eeprom_read_byte(EEPROM_LIGHTS_ON_HOUR_ADDR);
		lights_on_minute = eeprom_read_byte(EEPROM_LIGHTS_ON_MINUTE_ADDR);
		lights_on_second = eeprom_read_byte(EEPROM_LIGHTS_ON_SECOND_ADDR);
		
		lights_off_hour = eeprom_read_byte(EEPROM_LIGHTS_OFF_HOUR_ADDR);
		lights_off_minute = eeprom_read_byte(EEPROM_LIGHTS_OFF_MINUTE_ADDR);
		lights_off_second = eeprom_read_byte(EEPROM_LIGHTS_OFF_SECOND_ADDR);
		
		fan_mode = eeprom_read_byte(EEPROM_FAN_MODE_ADDR);
		
		internal_fan_on_temp = eeprom_read_byte(EEPROM_INTERNAL_FAN_ON_TEMP_ADDR);
		internal_fan_off_temp = eeprom_read_byte(EEPROM_INTERNAL_FAN_OFF_TEMP_ADDR);
		
		/* heater defaults */
		heater_mode = eeprom_read_byte(EEPROM_HEATER_MODE_ADDR);
		heater_on_temp = eeprom_read_byte(EEPROM_HEATER_ON_TEMP_ADDR);
		heater_off_temp = eeprom_read_byte(EEPROM_HEATER_OFF_TEMP_ADDR);
		//  heater on-time defaults
		heater_on_time.hour = eeprom_read_byte(EEPROM_HEATER_ON_HOUR_ADDR);
		heater_on_time.minute = eeprom_read_byte(EEPROM_HEATER_ON_MINUTE_ADDR);
		heater_on_second = 0;
        //  heater off time defaults
        heater_off_time.hour = eeprom_read_byte(EEPROM_HEATER_OFF_HOUR_ADDR);
        heater_off_minute = eeprom_read_byte(EEPROM_HEATER_OFF_MINUTE_ADDR);
        heater_off_time.second = 0;
	}
	else {
		//	we have not previously initialized the defaults in EEPROM
		/*	set lights on/off defaults in EEPROM and set our working variables*/
		eeprom_write_byte(EEPROM_LIGHTS_ON_HOUR_ADDR, EEPROM_DEFAULT_LIGHTS_ON_HOUR);
		eeprom_write_byte(EEPROM_LIGHTS_ON_MINUTE_ADDR, EEPROM_DEFAULT_LIGHTS_ON_MINUTE);
		eeprom_write_byte(EEPROM_LIGHTS_ON_SECOND_ADDR, EEPROM_DEFAULT_LIGHTS_ON_SECOND);
		lights_on_hour = EEPROM_DEFAULT_LIGHTS_ON_HOUR;
		lights_on_minute = EEPROM_DEFAULT_LIGHTS_ON_MINUTE;
		lights_on_second = EEPROM_DEFAULT_LIGHTS_ON_SECOND;
		
		eeprom_write_byte(EEPROM_LIGHTS_OFF_HOUR_ADDR, EEPROM_DEFAULT_LIGHTS_OFF_HOUR);
		eeprom_write_byte(EEPROM_LIGHTS_OFF_MINUTE_ADDR, EEPROM_DEFAULT_LIGHTS_OFF_MINUTE);
		eeprom_write_byte(EEPROM_LIGHTS_OFF_SECOND_ADDR, EEPROM_DEFAULT_LIGHTS_OFF_SECOND);
		lights_off_hour = EEPROM_DEFAULT_LIGHTS_OFF_HOUR;
		lights_off_minute = EEPROM_DEFAULT_LIGHTS_OFF_MINUTE;
		lights_off_second = EEPROM_DEFAULT_LIGHTS_OFF_SECOND;
		
		/*	fan mode	*/
		eeprom_write_byte(EEPROM_FAN_MODE_ADDR, EEPROM_DEFAULT_FAN_MODE);
		fan_mode = EEPROM_DEFAULT_FAN_MODE;
		
		/*	internal (case) fan */
		eeprom_write_byte(EEPROM_INTERNAL_FAN_ON_TEMP_ADDR, EEPROM_DEFAULT_INT_FAN_ON_TEMP);
		eeprom_write_byte(EEPROM_INTERNAL_FAN_OFF_TEMP_ADDR, EEPROM_DEFAULT_INT_FAN_OFF_TEMP);
		internal_fan_on_temp = EEPROM_DEFAULT_INT_FAN_ON_TEMP;
		internal_fan_off_temp = EEPROM_DEFAULT_INT_FAN_OFF_TEMP;
		
		/*	heaters */
		eeprom_write_byte(EEPROM_HEATER_MODE_ADDR, EEPROM_DEFAULT_HEATER_MODE);
		eeprom_write_byte(EEPROM_HEATER_ON_TEMP_ADDR, EEPROM_DEFAULT_HEATER_ON_TEMP);
		eeprom_write_byte(EEPROM_HEATER_OFF_TEMP_ADDR, EEPROM_DEFAULT_HEATER_OFF_TEMP);
		eeprom_write_byte(EEPROM_HEATER_ON_HOUR_ADDR, EEPROM_DEFAULT_HEATER_ON_HOUR);
		eeprom_write_byte(EEPROM_HEATER_ON_MINUTE_ADDR, EEPROM_DEFAULT_HEATER_ON_MINUTE);
		eeprom_write_byte(EEPROM_HEATER_OFF_HOUR_ADDR, EEPROM_DEFAULT_HEATER_OFF_HOUR);
		eeprom_write_byte(EEPROM_HEATER_OFF_MINUTE_ADDR, EEPROM_DEFAULT_HEATER_OFF_MINUTE);
		heater_mode = EEPROM_DEFAULT_HEATER_MODE;
		heater_on_temp = EEPROM_DEFAULT_HEATER_ON_TEMP;
		heater_off_temp = EEPROM_DEFAULT_HEATER_OFF_TEMP;
		heater_on_hour = EEPROM_DEFAULT_HEATER_ON_HOUR;
		heater_on_minute = EEPROM_DEFAULT_HEATER_ON_MINUTE;
		heater_off_hour = EEPROM_DEFAULT_HEATER_OFF_HOUR;
		heater_off_minute = EEPROM_DEFAULT_HEATER_OFF_MINUTE;
		
		
		//	mark that we've set the defaults
		eeprom_write_byte(EEPROM_INIT_COMPLETE_ADDR,EEPROM_INIT_COMPLETE_CODE);				
	}
}

int main(void)
{
	user_command_state = IDLE;
	//	initialize the I2C bus and let everything stabilize
	i2cInit();
	_delay_ms(100);
	lcd_init(void);							//	initialize the LCD
	_delay_ms(50);							//	allow LCD to initialize fully
	//	print welcome message
	lcd_clrscr();
	lcd_puts("Welcome to Greenhouse3\n");
	lcd_puts("(c) 2012 Alan Duncan");
	_delay_ms(2000);
	//	initialize the RTC
	ds1307_init(kDS1307Mode24HR);
	//	initialize the fans
	mcp23017_init(&fan_controller,0b000);
	init_fans(&fan_controller);
	//	initialize the serial bus
	uartInit(9600);
	//	initialize the lights
	_init_light_specs();
	init_lights(&light_specs);
	//	set the time and start the one-second interrupt
	time_init(&clock,55,59,4);
	_init_pulse();
	//	global flag when we update the time
	time_updated = 0;
	
	while(1) {
		//	if our 1-sec interrupt triggered, then perform period actions
		if( time_updated == 1)
		{
			gotolcd(0);                     // Set Cursor Line-1 
			sprintf(lcdbuf,"ET-BASE AVR....."); // Display Line-1 
			printlcd();
			//	keep track of our time
			time_update(&clock);
			//update_fans(&clock, &fan_controller);
			update_lights(&clock,&light_specs);
			//	display the approximate time
			sprintf(buffer,"%02d:%02d:%02d ",clock.hour,clock.minute,clock.second);
			lcd_clear_print(buffer);
			
			//	display the status of our lights
			_display_light_state();
			
			//	update fans and lights based on the current time
			update_fans(&clock, &fan_controller);
			update_lights(&clock,&light_specs);
			
			/*	Display second line content */
			/*	Alternating between the current fan time
				and the fan pattern	*/
			serial_lcd_set_line(2);
			if( clock.second % 6 < 3)
				_display_fan_time();
			else
				_display_fan_pattern();
			
			//	reset global update flag
			time_updated = 0;
		}
	}
}

void _init_light_specs()
{
	light_specs.bank_count = 5;			//	we have 5 banks of lights now
	light_specs.bank_latency = 5;		//	5 seconds between turning banks on/off
	light_specs.light_on_hour = 5;		//	turn lights on at 5 AM
	light_specs.light_off_hour = 20;	//	turn lights off at 8 PM
}

void _init_pulse()
{
	TIMSK1 |= (1 << TOIE1);	// Enable overflow interrupt 
	sei();					// Enable global interrupts
	
	TCNT1 = kTimerReloadConstant;	// Pre-load timer with pre-calculated value			 

	TCCR1B |= (1 << CS12);	//	setup timer at F_CPU/256
}

ISR(TIMER1_OVF_vect) 
{ 
	time_updated = 1;
	TCNT1  = kTimerReloadConstant; // Reload timer with pre-calculated value 
}

void _display_fan_pattern()
{
	/*	Display the fan pattern - dots for position off, asterisk for on */
	s08 bit = 12;
	while( --bit > -1 )
	{
		if( fan_controller.data & (1<<bit) )
			serial_lcd_write_byte(0b00101010);
		else
			serial_lcd_write_byte(0b10100101);
	}		
}

void _display_fan_time()
{
	u08 target = fan_target(&clock);
	u08 current = fan_current();
	sprintf(buffer,"FAN %02d/%03d",current,target);
	serial_lcd_write_string(buffer);
}

void _display_light_state()
{
	LightState state = light_specs.state;
	if( state == LightStateOff )
		serial_lcd_write_string("LT OFF");
	else if( state == LightStateOn )
		serial_lcd_write_string("LT ON");
	else if( state == LightStateOnTransition )
		serial_lcd_write_string("LT ON-X");
	else if( state == LightStateOffTransition )
		serial_lcd_write_string("LT OFF-X");
}

/*  Keypad and commands */

void process_keypad() {
    if( keypad_char != KEYPAD_CHAR_PROCESSED )
    {
        if( cmd_temp == CommandCodeFirstChar )
        {
            cls();
            _delay_ms(2);
        }   /* first character */
        //  echo the character to the display
        uartSendByte(keypad_char);
		keypad_char = KEYPAD_CHAR_PROCESSED;
		
    }   /* keypad_char != processed */
    if( cmd_temp < CommandCodeIncomplete )
    {
        cls();
        _delay_ms(2);
        switch( cmd_temp )
        {
            case CMD_RTC_SET_TIME:
            {
                user_command_state = ST_RTC_TIME_SEL;
                cls();
                lcd_print_str_consecutively("Set clock hour","(000 - 023)");
                user_command_state = ST_RTC_HOUR_CONF;
                break;
            }   /*  CMD_RTC_SET_TIME */
            case CMD_SET_LIGHTS_ON_TIME:
            {
                user_command_state = ST_LTS_ON_TIME_SEL;
                cls();
                lcd_print_str_consecutively("Set lts ON hour","(000 - 023)");
                user_command_state = ST_LTS_ON_HOUR_CONF;
                break;
            }   /* CMD_SET_LIGHTS_ON_TIME */
            case CMD_SET_LIGHTS_OFF_TIME:
            {
                user_command_state = ST_LTS_OFF_TIME_SEL;
                cls();
                lcd_print_str_consecutively("Set lts OFF hr","(000 - 023)");
                user_command_state = ST_LTS_OFF_HOUR_CONF;
                break;
            }   /* CMD_SET_LIGHTS_OFF_TIME */
            case CMD_SET_HEATER_MODE:
            {
                user_command_state = ST_HEAT_MODE_SEL;
                cls();
                lcd_print_str_consecutively("Set heat mode","(000 - 002)");
                user_command_state = ST_HEAT_MODE_CONF:
                break;
            }   /* CMD_SET_HEATER_MODE */
            case CMD_SET_HEATER_ON_TEMP:
            {
                user_command_state = ST_HEAT_TEMP_ON_SEL;
                cls();
                lcd_print_str_consecutively("Set heat ON temp","(000 - 070)C");
                user_command_state = ST_HEAT_TEMP_ON_CONF;
                break;
            }   /* CMD_SET_HEATER_ON_TEMP */
            case CMD_SET_HEATER_OFF_TEMP:
            {
                user_command_state = ST_HEAT_TEMP_OFF_SEL;
                cls();
                lcd_print_str_consecutively("Set heat OFF tmp","(000 - 070)C");
                user_command_state = ST_HEAT_TEMP_OFF_CONF;
                break;
            }   /*  CMD_SET_HEATER_OFF_TEMP */
            case CMD_MANUAL_HEATER_ON:
            {
                user_command_state = ST_HEAT_MAN_ON_CONFD;
                heater_mode = k_heater_mode_manual;
                heaters_on();
                user_command_state = IDLE;
                break
            }   /* CMD_MANUAL_HEATER_ON */
            case CMD_MANUAL_HEATER_OFF:
            {
                user_command_state = ST_HEAT_MAN_OFF_CONFD;
                heater_mode = k_heater_mode_manual;
                heaters_off();
                user_command_state = IDLE;
                break;
            }   /*  CMD_MANUAL_HEATER_OFF   */
            case CMD_SET_FAN_MODE:
            {
                user_command_state = ST_FAN_MODE_SEL;
                cls();
                lcd_print_str_consecutively("Set fan mode","(000 - 002)");
                user_command_state = ST_FAN_MODE_CONF;
                break;
            }   /* CMD_SET_FAN_MODE */
            case CMD_SET_INTERNAL_FAN_MODE:
            {
                user_command_state = ST_INT_FAN_MODE_SEL;
                cls();
                lcd_print_str_consecutively("Set int fan mode","(000 - 001)");
                user_command_state = ST_INT_FAN_MODE_CONF;
                break;
            }   /* CMD_SET_INTERNAL_FAN_MODE */
            case CMD_SET_INTERNAL_FAN_ON_TEMP:
            {
                user_command_state = ST_INT_FAN_ON_TEMP_SEL;
                cls();
                lcd_print_str_consecutively("Set ifan on T","(020 - 070)");
                user_command_state = ST_INT_FAN_ON_TEMP_CONF;
                break;
            }
            case CMD_SET_INTERNAL_FAN_OFF_TEMP:
            {
                user_command_state = ST_INT_FAN_OFF_TEMP_SEL;
                cls();
                lcd_print_str_consecutively("Set ifan off T","(020 - 070)");
                user_command_state = ST_INT_FAN_OFF_TEMP_CONF;
                break;
            }   /* ST_INT_FAN_ON_TEMP_SEL */
            default:
            {
                switch( user_command_state )
                {
                    case ST_RTC_HOUR_CONF:
                    {
                        user_command_state = ST_RTC_HOUR_CONFD;
                        //  the cmd_temp is the hour
                        ds1307_set_hours(cmd_temp);
                        cls();
                        lcd_print_consecutively("Set clock mins","(000 - 059)");
                        user_command_state = ST_RTC_MIN_CONF;
                        break;
                    }   /*  ST_RTC_HOUR_CONF    */
                    case ST_RTC_MIN_CONF:
                    {
                        user_command_state = ST_RTC_MIN_CONFD;
                        ds1307_set_minutes(cmd_temp);
                        cls();
                        lcd_print_consecutively("Set clock secs","(000 - 059)");
                        user_command_state = ST_RTC_SEC_CONF;
                        break;
                    }   /*  ST_RTC_MIN_CONF */
                    case ST_RTC_SEC_CONF:
                    {
                        user_command_state = ST_RTC_SEC_CONFD;
                        ds1307_set_seconds(cmd_temp);
                        lcd_OK_wait();
                        user_command_state = IDLE;
                        lcd_clear_print_ready();
                        break;
                    }   /*  ST_RTC_SEC_CONF */
                    case ST_FAN_MODE_CONF:
                    {
                        user_command_state = ST_FAN_MODE_CONFD;
                        fans_set_mode(cmd_temp);
                        lcd_OK_wait();
                        user_command_state = IDLE;
                        lcd_clear_print_ready();
                        break;
                    }   /*  ST_FAN_MODE_CONF    */
                    case ST_LTS_ON_HOUR_CONF:
                    {
                        user_command_state = ST_LTS_ON_HOUR_CONFD;
                        lights_set_on_hour(temp_cmd);
                        cls();
                        lcd_print_consecutively("Set lt ON min","(000 - 059)");
                        user_command_state = ST_LTS_ON_MIN_CONF;
                        break;
                    }   /* ST_LTS_ON_HOUR_CONF */
                    case ST_LTS_ON_MIN_CONF:
                    {
                        user_command_state = ST_LTS_ON_MIN_CONFD;
                        lights_set_on_minute(temp_cmd);
                        lights_set_on_second(30);
                        lcd_OK_wait();
                        user_command_state = IDLE;
                        lcd_clear_print_ready();
                        break;  
                    }   /* ST_LTS_ON_MIN_CONF */
                    case ST_LTS_OFF_HOUR_CONF:
                    {
                        user_command_state = ST_LTS_OFF_HOUR_CONFD;
                        lights_set_off_hour(temp_cmd);
                        cls();
                        lcd_print_consecutively("Set ltOFF min","(000 - 059)");
                        user_command_state = ST_LTS_OFF_MIN_CONF;
                        break;  
                    }   /* ST_LTS_OFF_HOUR_CONF */
                    case ST_LTS_OFF_MIN_CONF:
                    {
                        user_command_state - ST_LTS_OFF_MIN_CONFD;
                        lights_set_off_minute(temp_cmd);
                        lcd_OK_wait();
                        user_command_state = IDLE;
                        lcd_clear_print_ready();
                        break;  /*  ST_LTS_OFF_MIN_CONF */
                    }
                    
                }   /* command state switch
            }   /*  default command handler */
        }   /* command switch */
    }   /*  command is not incomplete */
}   /*  process_keypad() */


#define SERIAL_LCD_COMMAND				0xFE
#define SERIAL_LCD_LINE_ONE				0x80
#define SERIAL_LCD_LINE_TWO				0xC0
#define SERIAL_LCD_CLEAR_SCREEN			0x01
#define SEND_COMMAND uartSendByte(SERIAL_LCD_COMMAND)


void cls() {
	SEND_COMMAND;
	uartSendByte(SERIAL_LCD_CLEAR_SCREEN);
	_delay_ms(2);
}

void lcd_set_line(u08 line)
{
	SEND_COMMAND;
	uartSendByte((line==1)?SERIAL_LCD_LINE_ONE:SERIAL_LCD_LINE_TWO);
}
 
void lcd_clear_print(char *s)
{
	cls();
	uartSendStr(s);
}

void lcd_clear_print_ready()
{
	lcd_clear_print("Ready");
}

void lcd_print_wait(char *s)
{
	lcd_clear_print(s);
	_delay_ms(1000);
	cls();
}

void lcd_OK_wait()
{
	lcd_print_wait("OK");
}

void lcd_cancel_wait()
{
	lcd_print_wait("CANCELLED");
}

void lcd_print_str_consecutively(char *s1, char *s2)
{
	LCD_PRINT_CONSECUTIVELY(s1,s2);
}