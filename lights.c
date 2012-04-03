/*
 * lights.c
 *
 * Created: 12/26/2011 4:28:17 AM
 *  Author: Owner
 */ 

#include "lights.h"
#include <avr/io.h>



/************************************************************************/
/* FUNCTION PROTOTYPES (private functions)                              */
/************************************************************************/
void _set_bank_state(u08 index, BOOL state );
void _initiate_transition_on(LightSpecs *specs, BOOL is_on);
void _handle_transition_on(Time *clock,LightSpecs *specs,BOOL is_on);

/************************************************************************/
/* PORT DEFINITIONS                                                     */
//	There are two banks of relays each having 4 units
//	The IDC connector labeled RELAY BOARD 1 is the LOWER (L) bank
//	The IDC connector labeled RELAY BOARD 2 is the UPPER (H) bank
//	PD2 = lower relay 0
//	PD3 = lower relay 1
//	PD4 = lower relay 2
//	PD5 = lower relay 3
//	PD6 = upper relay 0 (labeled L_O_4)
//	PB6 = upper relay 1 (labeled H_O_0)	(heater 0)
//	PB7 = upper relay 2 (labeled H_O_1) (heater 1)
/************************************************************************/
void init_lights(LightSpecs *specs)
{
	PIND = 0b00000000;
	specs->state = LightStateOff;
	DDRD |= 0b01111100;				//	PD2-6 are outputs
	PORTD &= ~(0b11111100);			//	turn off all on PORTD
}

//
//	NOTE:  Do NOT call this function more than 1x/sec
//
void update_lights( Time *clock, LightSpecs *specs)
{
	switch( specs->state )
	{
		case LightStateOff:
		{
			//	turn lights on when we've reached the spec'd time
			if( clock->hour == specs->light_on_hour && clock->minute == 0)
			{
				//	begin process of turning on lights
				_initiate_transition_on(specs,TRUE);
			}
			break;
		}
		case LightStateOnTransition:
			//	if we're still in transition, continue the transition
			_handle_transition_on(clock,specs,TRUE);
			break;
		case LightStateOn:
		{
			//	if we've reached the time to turn off lights, turn them off
			if( clock->hour == specs->light_off_hour && clock->minute == 0)
			{
				_initiate_transition_on(specs,FALSE);
			}
			break;
		}
		case LightStateOffTransition:
			_handle_transition_on(clock,specs,FALSE);
			break;
	}	
}

void lights_set_on_hour(void)
{
}

void lights_set_on_minute(void)
{
}

void lights_set_on_second(void)
{
}
void lights_set_off_hour(void)
{
}
void lights_set_off_minute(void)
{
}
void lights_set_off_second(void);
{
}

void _handle_transition_on(Time *clock,LightSpecs *specs,BOOL is_on)
{
	specs->transition_time++;			//	increment our transition time count
	//	if we reached the transition time
	if( specs->transition_time == specs->bank_latency )
	{
		//	advance to the next bank of lights
		specs->index++;
		//	reset our transition time
		specs->transition_time = 0;
		//	set our state if all our lights are managed
		if( specs->index >= specs->bank_count )
		{
			specs->index = 0;
			specs->state = (is_on)?LightStateOn:LightStateOff;
		}
		else
		{
			//	set the bank to the ON/OFF state
			_set_bank_state(specs->index,(is_on));
		}
	}		
}

//
//	Begins the transition ON/OFF
//
//	Parameter LightSpecs *specs A pointer to the LightSpecs object
//	Parameter BOOL is_on If TRUE, turn on the lights; otherwise, turn lights off
//
void _initiate_transition_on(LightSpecs *specs, BOOL is_on)
{
	specs->state = (is_on)?LightStateOnTransition:LightStateOffTransition;
	specs->index = 0;
	specs->transition_time = 0;
	_set_bank_state(specs->index, is_on);	//	turn the first bank ON
}

//	
//	Turns specified light bank on or off
//
///	bank 0 = PD2
//	bank 1 = PD3
//	bank 2 = PD4
//	bank 3 = PD5
//	bank 4 = PD6
//	We also have connections on PA0,PA1 and PB0 that should be turned OFF for now
//	PD5 is not connected (?!)
//
void _set_bank_state(u08 index, BOOL state )
{
	u08 pin = index + 2;
	if( state )
		PORTD |= (1<<pin);
	else
		PORTD &= ~(1<<pin);
}
