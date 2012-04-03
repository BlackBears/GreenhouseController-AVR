
#define CMD_IDLE						000

/*	SETTING RTC				*/
#define CMD_RTC_SET_TIME				100

/*	READING RTC				*/
#define CMD_RTC_DISPLAY_TIME			104

/*	SETTING EVENT TIMES		*/
#define CMD_SET_LIGHTS_ON_TIME			201
#define CMD_SET_LIGHTS_OFF_TIME			202

/*	HEATER CONTROL			*/
#define CMD_SET_HEATER_MODE				203
#define CMD_SET_HEATER_ON_TEMP			204
#define CMD_SET_HEATER_OFF_TEMP			205
#define CMD_MANUAL_HEATER_ON			206
#define CMD_MANUAL_HEATER_OFF			207

/*	TEMPERATURES			*/
#define CMD_DISPLAY_AMBIENT_TEMP		301
#define CMD_DISPLAY_SOIL_TEMP			302
#define CMD_DISPLAY_CASE_TEMP			303

/*	FANS					*/
#define CMD_SET_FAN_MODE				401
#define CMD_MANUAL_FAN_ON				402
#define CMD_MANUAL_FAN_OFF				403

/*	INTERNAL FAN			*/
#define CMD_SET_INTERNAL_FAN_MODE       500
#define CMD_SET_INTERNAL_FAN_ON_TEMP	501
#define CMD_SET_INTERNAL_FAN_OFF_TEMP	502
#define CMD_MANUAL_INTERNAL_FAN_ON		503
#define CMD_MANUAL_INTERNAL_FAN_OFF		504



#define CMD_MANUAL_ON		101
#define CMD_PROFILE_ON		201	
#define CMD_MANUAL_OFF		301

#define CMD_READ_THERMO0	400		//	read the top thermocouple
#define CMD_READ_THERMO1	401		//	read the bottom thermocouple
#define CMD_FAN_ON			410		//	command the cooling fan on
#define CMD_FAN_OFF			411		//	command the cooling fan off

#define CMD_READ_AMB_TEMP	500		//	read the internal temperature in the enclosure
#define CMD_SET_FAN_ON_T	501		//	set the setpoint for internal fan-on
#define CMD_SET_FAN_OFF_T	502		//	"" for fan-off
#define CMD_READ_FAN_ON_T	503		//	print fan on set point
#define CMD_READ_FAN_OFF_T	504		//	print fan off set point
#define CMD_FAN_RESET		505		//	reset fan temps to default