
#include "global.h"
#include "command.h"
#include <string.h>
#include <stdlib.h>
#include "uart-am168.h"

volatile char command_str[8];
char buffer[10];

_append_char(u08 in_char);

void command_init() {
    for(u08 i = 0; i < 8; i++ )
		command_str[i] = '\0';
}

CommandCode command_add_character(u08 in_char) {
    if( in_char != '#' )
    {
        //  if incoming char is not '#', just add it and return
        _append_char(in_char);
		if( strlen(command_str) == 1 )
		{
			return CommandCodeFirstChar;
		}			
    }
    else
    {
        //  user pressed '#'
        if( strlen(command_str) <= 4 )
        {   
            CommandCode temp = atoi(command_str);
            command_init();
            return temp;
        }
        else
        {
            command_init();
			
            return CommandCodeInvalid;
        }
    }
    return CommandCodeIncomplete;
}

_append_char(u08 in_char) {
	u08 i = strlen(command_str);
	command_str[i] = in_char;
	command_str[i+1] = '\0';
}