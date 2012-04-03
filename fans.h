/*!	
 *	\file		fans.h
 *	\brief		Header file for fan control
 *	\details	Header file for the fan control module of the greenhouse controller
 *	\author		Alan K Duncan <duncan.alan@mac.com>
 *	\date		2011-12-27 14:14:00 CST
 *	\version	1.0
 */		

#ifndef FANS_H_
#define FANS_H_

#include "time.h"
#include "mcp23017.h"

volatile u08 fan_mode;
volatile u08 current_pattern_index;
volatile u08 current_fan_seconds;

/*!
 *	\fn			init_fans( MCP23017 *controller )
 *	\brief		Initializer
 *	\details	Initialize fans on the specified controller.  This method requires an MCP23017
 *				port expander with I2C interface.
 *	\param		controller A MCP23017 controller object.  
 *	\sa			mcp23017.h
 */
void init_fans( MCP23017 *controller );

/*!
 *	\fn			update_fans( Time *clock, MCP23017 *controller )
 *	\brief		Update fans
 *	\details	This method should be called every second to update the fan status.
 *	\param		clock A Time object used to update the fans
 *	\param		controller A MCP23017 controller object
 *	\sa			mcp23017.h
 *	\sa			time.h
 *	\warning	Do not call this method more often than 1x/sec or the timing of the
 *				fan state will be incorrect.
 */
void update_fans( Time *clock, MCP23017 *controller );

/*!
 *	\fn			fan_target( Time *clock )
 *	\brief		Returns fan target time
 *	\details	This method returns an integer representing how long, in seconds, the fan
 *				patterns will be executed.
 *	\param		clock The Time object used to compute the current target
 *	\return		u08 representing the number of seconds for the fan patterns at this time.
 */
u08 fan_target( Time *clock );

/*!
 *	\fn			u08 fan_current()
 *	\brief		Returns fan current time
 *	\details	This method returns an integer (u08) representing how long, in seconds,
 *				the current fan pattern has been executing.
 *	\return		u08 representing the number of seconds that the current fan pattern has been
 *				executing.
 */
u08 fan_current();

#warning  Method not implemented
void fans_set_mode(u08 mode);

#endif /* FANS_H_ */