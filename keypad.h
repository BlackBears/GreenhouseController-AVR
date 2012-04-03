/*! \file   keypad.h
 *  \brief  4x3 keypad reader using MCP23008 GPIO expander
 */
 //*****************************************************************************
 //
 // File name       :   keypad.h
 // Title           :   4x3 keypad reader using MCP23008 GPIO expander
 // Author          :   Alan K. Duncan <duncan.alan@mac.com>
 // Created         :   2012-01-02
 // Revised         :
 // Version         :   1.0
 // Target MCU      :   Any AVR
 //
 // This code is distributed under the MIT license
 //
 //*****************************************************************************
 
#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "global.h"
#include "mcp23008.h"

enum {
	KeypadStateNormal,
	KeypadStateButtonDown,
	KeypadStateWaitingRelease
};
typedef u08 KeypadState;

typedef struct {
	MCP23008 interface;
	KeypadState state;
} Keypad;

void keypad_init(Keypad *k,u08 address);
u08 keypad_read(Keypad *k);
u08 keypad_update(Keypad *k);
void _keypad_write_outputs(Keypad *k);
void _keypad_clear_interrupt(Keypad *k);

#endif /* KEYPAD_H_ */