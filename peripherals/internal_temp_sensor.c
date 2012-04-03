

#include "internal_temp_sensor.h"

u08 internal_temp_sensor_init()
{
    a2dInit();
}

u08 internal_temp_sensor_read()
{
    return a2dConvert8bit();
}