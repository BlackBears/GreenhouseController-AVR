/*
 * time.h
 *
 * Created: 12/27/2011 3:30:14 AM
 *  Author: Owner
 */ 


#ifndef TIME_H_
#define TIME_H_

#include "global.h"

typedef struct {
	u08 second;
	u08 minute;
	u08 hour;
	u08 day;
} Time;

void time_init( Time *time, u08 second, u08 minute, u08 hour);
void time_update( Time *time);



#endif /* TIME_H_ */