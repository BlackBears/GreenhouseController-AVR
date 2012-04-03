/*
 * time.c
 *
 * Created: 12/27/2011 3:30:00 AM
 *  Author: Owner
 */ 

#include "time.h"

void time_init( Time *time, u08 second, u08 minute, u08 hour)
{
	time->second = second;
	time->minute = minute;
	time->hour = hour;
}

void time_update( Time *time)
{
	u08 temp = time->second + 1;
	if( temp == 60 )
	{
		time->second = 0;
		temp = time->minute + 1;
		if( temp == 60 )
		{
			time->minute = 0;
			temp = time->hour + 1;
			if( temp == 24 )
				time->hour = 0;
			else
				time->hour = temp;
		}
		else
			time->minute = temp;
	}
	else
		time->second = temp;
}