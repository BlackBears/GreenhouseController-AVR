/*! \file		lights.h
 *	\brief		Manages lights for greenhouse controller project
 *	\details	This is the header file for the code that manages the lights for the greenhouse controller project.
 *	\author		Alan K Duncan <duncan.alan@mac.com>
 *	\date		2011-12-27 06-32-00 CST
 *	\version	1.0
 */
#ifndef LIGHTS_H_
#define LIGHTS_H_

#include "global.h"
#include "time.h"

enum {
	LightStateOff,
	LightStateOnTransition,
	LightStateOn,
	LightStateOffTransition
};
typedef u08 LightState;

typedef struct {
	u08 light_on_hour;		//	what time (hour) to turn on the lights
	u08 light_off_hour;		//	what time (hour) to turn off the lights
	u08 bank_count;			//	how many banks of lights do we have
	u08 bank_latency;		//	seconds between bank transitions
	LightState state;		//	our current state
	u08 index;				//	which indexed light bank is currently in transition
	u08 transition_time;	//	number of seconds on current transition step
} LightSpecs;

volatile u08 lights_on_hour;
volatile u08 lights_on_minute;
volatile u08 lights_on_second;
volatile u08 lights_off_hour;
volatile u08 lights_off_minute;
volatile u08 lights_off_second;

/*!
 *	\fn void init_lights(LightSpecs *specs)
 *	\brief Initializer
 *	\details This function initializes the light control functionality.
 *	\param	specs A pointer to the LightSpecs object that specifies the light behaviors
 */
void init_lights(LightSpecs *specs);

/*!
 *	\fn	void update_lights( Time *clock, LightSpecs *specs)
 *	\brief Manage the light bank status
 *	\details This function manages the status of the light banks.
 *	\param clock A pointer to the Time object containing our virtual clock
 *	\param specs A pointer to the LightSpecs object containing the light specifications
 *	\warning Only call this function once per second as the implementation assumes this update frequency.
 */
void update_lights( Time *clock, LightSpecs *specs);

#warning method not implemented
void lights_set_on_hour(void);
void lights_set_on_minute(void);
void lights_set_on_second(void);
void lights_set_off_hour(void);
void lights_set_off_minute(void);
void lights_set_off_second(void);

#endif /* LIGHTS_H_ */