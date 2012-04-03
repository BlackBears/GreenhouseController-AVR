/*
 * mcp23017.c
 *
 * Created: 12/23/2011 5:40:33 PM
 *  Author: Owner
 */ 

#include "mcp23017.h"
#include "i2c.h"
#include <util/delay.h>

#define MCP23017_BASE_ADDRESS 0x40

u08 mcp23017_data[2];

void mcp23017_init(MCP23017 *obj, uint8_t address)
{
	obj->address = MCP23017_BASE_ADDRESS + address;
	
	//	set addressing style
	mcp23017_write_register(obj,MCP23017_IOCON,0x00);
	
	obj->data = 0x0000;
	mcp23017_write_word(obj,MCP23017_IODIRA);	//	make all output
}
void mcp23017_write(MCP23017 *obj)
{
	mcp23017_write_word(obj,MCP23017_GPIOA);
}

void mcp23017_write_register( MCP23017 *obj, u08 reg, u08 data)
{
	
	mcp23017_data[0] = reg;
	mcp23017_data[1] = data;
	i2cMasterSendNI(obj->address,2,&mcp23017_data);
}

u08 mcp23017_read_register( MCP23017 *obj,u08 reg)
{
	mcp23017_data[0] = reg;
	i2cMasterSendNI(obj->address,1,&mcp23017_data);
	i2cMasterReceiveNI(obj->address,1,&mcp23017_data);
	return mcp23017_data[0];
}

void mcp23017_write_word( MCP23017 *obj, u08 reg )
{
	mcp23017_data[0] = reg;
	u16 data = obj->data;
	mcp23017_data[1] = (u08)data;
	mcp23017_data[2] = (u08)(data>>8);
	i2cMasterSendNI(obj->address,3,&mcp23017_data);
}