/*
 * fans.c
 *
 * Created: 12/26/2011 4:06:37 AM
 *  Author: Owner
 */ 

#include "global.h"
#include "fans.h"
#include "mcp23017.h"
#include <avr/pgmspace.h>
#include "seriallcd.h"

#define MAX_PATTERN_INDEX 23

//	fan duration in seconds by hour starting with 0 (MN)
static uint8_t fan_duration[] PROGMEM = {
			5,5,5,5,5,7,10,20,40,60,80,120,220,220,220,220,120,100,80,60,40,20,10,7		
};
//	array of patterns to use for the fans, 1 indicates fan ON, 0 = fan OFF for that bit position
static u16 fan_pattern[32] PROGMEM = {
	0b0000000000000001,	
	0b0000000000000010,
	0b0000000000000100,
	0b0000000000001000,
	0b0000000000010000,
	0b0000000000100000,
	0b0000000001000000,
	0b0000000010000000,
	0b0000000100000000,
	0b0000001000000000,
	0b0000010000000000,
	0b0000100000000000,
	0b0000000000000011,
	0b0000000000000110,
	0b0000000000001100,
	0b0000000000011000,
	0b0000000000110000,
	0b0000000001100000,
	0b0000000011000000,
	0b0000000110000000,
	0b0000001100000000,
	0b0000011000000000,
	0b0000110000000000,
};



void init_fans( MCP23017 *controller )
{
	current_fan_seconds = 0;
	current_pattern_index = 0;
	controller->data = pgm_read_word( &fan_pattern[0] );
	mcp23017_write(controller);
}

void update_fans( Time *clock, MCP23017 *controller )
{
	//	update our current seconds;
	current_fan_seconds++;
	
	//	we update the duration each hour
	u08 target_seconds = pgm_read_byte( &fan_duration[clock->hour] );
	//	if we've reached the target duration for the pattern, time to advance the pattern
	if( current_fan_seconds == target_seconds )
	{
		//	fan has reached its target duration
		current_fan_seconds = 0;
		current_pattern_index++;
		//	loop pattern to beginning if we reached the end
		if( current_pattern_index > MAX_PATTERN_INDEX )
			current_pattern_index = 0;
		//	write the current pattern to the fan controller
		controller->data = pgm_read_word( &fan_pattern[current_pattern_index] );
	
		mcp23017_write(controller);
	}		
}

u08 fan_target( Time *clock )
{
	return  pgm_read_byte( &fan_duration[clock->hour] );
}

u08 fan_current()
{
	return current_fan_seconds;
}

void fans_set_mode(u08 mode)
{
    return;
}