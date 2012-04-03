

#ifndef __COMMAND_H_
#define __COMMAND_H_

#include "global.h"

enum {
    CommandCodeIncomplete = 0xFD00,
    CommandCodeInvalid = 0xFE00,
	CommandCodeFirstChar = 0xFF00
};
typedef u16 CommandCode;

void command_init();

//  adds a character to the command
//  if command code is incomplete (<3 characters) returns CommandCodeIncomplete
//  if command code is invalid returns CommandCodeInvalid
//  otherwise returns the u16 command code
//
CommandCode command_add_character(u08 in_char);

#endif // __COMAND_H_H
