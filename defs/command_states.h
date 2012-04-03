

#define IDLE		000

/*  heater  */
#define ST_HEAT_MODE_SEL        100     //  heat mode selected
#define ST_HEAT_MODE_CONF       101     //  confirm heat mode   (waiting for user to enter mode)
#define ST_HEAT_MODE_CONFD      102     //  user has entered a heat mode
#define ST_HEAT_TEMP_ON_SEL     103     //  heat ON temp selected
#define ST_HEAT_TEMP_ON_CONF    104     //  heat ON temp confirm (waiting for user to enter temp)
#define ST_HEAT_TEMP_ON_CONFD   105     //  heat ON temp confirmed (user has entered value now)
#define ST_HEAT_TEMP_OFF_SEL    106     //  heat OFF temp selected
#define ST_HEAT_TEMP_OFF_CONF   107     //  heat OFF temp confirmed
#define ST_HEAT_TEMP_OFF_CONFD  108     //  heat OFF temp confirmed
#define ST_HEAT_MAN_ON_CONFD    109     //  heat manual on confirmed
#define ST_HEAT_MAN_OFF_CONFD   110     //  heat manual off confirmed

/*  RTC     */
#define ST_RTC_TIME_SEL         200     //  set RTC time selected
#define ST_RTC_HOUR_CONF        201     //  waiting for hour
#define ST_RTC_HOUR_CONFD       202     //  user has entered hour
#define ST_RTC_MIN_CONF         204     //  waiting for minute
#define ST_RTC_MIN_CONFD        205     //  user has entered minute
#define ST_RTC_SEC_CONF         206     //  waiting for second
#define ST_RTC_SEC_CONFD        207     //  user has entered second

/*  FANS    */
#define ST_FAN_MODE_SEL         300     //  set fan mode selected
#define ST_FAN_MODE_CONF        301     //  waiting for fan mode
#define ST_FAN_MODE_CONFD       302     //  user has entered a fan mode

/*  TEMPERATURE DISPLAY     */
#define ST_AMB_TEMP_DISP_SEL    400     //  user requested display ambient temp
#define ST_SOIL_TEMP_DISP_SEL   401     //  user requested display soil temp
#define ST_CASE_TEMP_DISP_SEL   402     //  user requested display case temperature

/*  LIGHTS      */
#define ST_LTS_ON_TIME_SEL      400     //  user requested set lights ON time
#define ST_LTS_ON_HOUR_CONF     401     //  waiting for user to enter the lights on hour
#define ST_LTS_ON_HOUR_CONFD    402     //  user has entered the lights on hour
#define ST_LTS_ON_MIN_CONF      403     //  waiting for user to enter the lights on minute
#define ST_LTS_ON_MIN_CONFD     404     //  user has entered the lights on minute
#define ST_LTS_OFF_TIME_SEL     405     //  user requested set lights OFF time
#define ST_LTS_OFF_HOUR_CONF    406     //  waiting for user to enter the lights off hour
#define ST_LTS_OFF_HOUR_CONFD   407     //  user has entered the lights off hour
#define ST_LTS_OFF_MIN_CONF     408     //  waiting for user to enter the lights off minute
#define ST_LTS_OFF_MIN_CONFD    409     //  user has entered the lights off minute

/*  INTERNAL FAN    */
#define ST_INT_FAN_MODE_SEL         500     //  internal fan mode selected
#define ST_INT_FAN_MODE_CONF        501     //  waiting for fan mode
#define ST_INT_FAN_MODE_CONFD       502     //  user has entered the internal fan mode
#define ST_INT_FAN_ON_TEMP_SEL      503     //  int fan on temp selected
#define ST_INT_FAN_ON_TEMP_CONF     504     //  waitinf for fan on temp
#define ST_INT_FAN_ON_TEMP_CONFD    505     //  user has entered internal fan on temp
#define ST_INT_FAN_OFF_TEMP_SEL     506     //  int fan off temp selected
#define ST_INT_FAN_OFF_TEMP_CONF    507     //  waiting for internal fan off temp
#define ST_INT_FAN_OFF_TEMP_CONFD   508     //  user has entered internal fan off temp


#define M_ON_SEL			101
#define	M_ON_DISP1			102
#define	M_ON_TEM			103
#define M_ON				104
#define	M_OFF_SEL			201
#define M_OFF_DISP			202
#define	M_OFF_CONF			203
#define	M_OFF_CONFD			204

#define P_ON_SEL			301
#define P_ON_CONF			302
#define P_ON_CONFD			303

#define SET_FAN_ON_SEL		501
#define SET_FAN_ON_CONF		502
#define SET_FAN_ON_CONFD	503

#define SET_FAN_OFF_SEL		510
#define SET_FAN_OFF_CONF	512
#define SET_FAN_OFF_CONFD	513

