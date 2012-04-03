/*
 * heater.h
 *
 * Created: 3/28/2012 5:22:22 AM
 *  Author: Owner
 */ 


#ifndef HEATER_H_
#define HEATER_H_

#include "../global.h"

enum {
	k_heater_mode_cyclic,
	k_heater_mode_temperature,
	k_heater_mode_manual
};

volatile Time heater_on_time;
volatile Time heater_off_time;

volatile u08 heater_mode;
volatile u08 heater_on_temp;
volatile u08 heater_off_temp;
volatile u08 heater_on_hour;
volatile u08 heater_on_minute;
volatile u08 heater_off_hour;
volatile u08 heater_off_minute;

#warning Methods not implemented
void heaters_on();
void heaters_off();

#endif /* HEATER_H_ */