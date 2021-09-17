//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "fire_alarm.h"
#include "siren.h"
#include "strobe_light.h"
#include "user_interface.h"
#include "code.h"
#include "date_and_time.h"
#include "matrix_keypad.h"
#include "hum_suelo_sensor.h"
#include "temp_hum_sensor.h"
#include "servo_motor.h"
#include "electrovalvula.h"

//=====[Declaration of private defines]======================================

#define TEMPERATURE_C_LIMIT_ALARM            23.0
#define HUMIDITY_C_LIMIT_ALARM               25.0
#define STROBE_TIME_HUM                      1000
#define STROBE_TIME_OVER_TEMP                500
#define STROBE_TIME_OVER_HUM_AND_OVER_TEMP   100

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalIn alarmTestButton(BUTTON1);

//=====[Declaration and initialization of private global variables]============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool overTemperatureDetected       = OFF;
static bool overHumidityDetected          = OFF;   
static bool overTemperatureDetectorState  = OFF;
static bool overHumidityDetectorState     = OFF; 

//=====[Declarations (prototypes) of private functions]========================

static void fireAlarmActivationUpdate();
static void fireAlarmDeactivationUpdate();
static void fireAlarmDeactivate();
static int  fireAlarmStrobeTime();

//=====[Implementations of public functions]===================================

void fireAlarmInit()
{
    sirenInit();
    strobeLightInit();
    humedadSensorInit();
    servoControlInit();	
    electrovalvulaInit();
	alarmTestButton.mode(PullDown); 
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
    fireAlarmDeactivationUpdate();
    sirenUpdate( fireAlarmStrobeTime() );
    strobeLightUpdate( fireAlarmStrobeTime() );	
}

bool overTemperatureDetectorStateRead()
{
    return overTemperatureDetectorState;
}

bool overHumidityDetectorStateRead()
{
    return overHumidityDetectorState;
}

bool overTemperatureDetectedRead()
{
    return overTemperatureDetected;
}

bool overHumidityDetectedRead()
{
	return overHumidityDetected;
}
//=====[Implementations of private functions]==================================

static void fireAlarmActivationUpdate()
{
    
    humedadSensorUpdate();
	
    overTemperatureDetectorState = tempSensorReadCelsius() > TEMPERATURE_C_LIMIT_ALARM;
    
    overHumidityDetectorState = humedadSensorRead() < HUMIDITY_C_LIMIT_ALARM;  

    if ( overTemperatureDetectorState ) 
    {
        setDutyCycle( 0.0021 );
        overTemperatureDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
        
    }else{
    	
        servoControlInit();	
    }
    
    
    if( overHumidityDetectorState )
    {
        overHumidityDetected = ON; 
        electrovalvulaStateWrite(ON);
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
         	
    }else {
        electrovalvulaStateWrite(OFF);
    }

    if( alarmTestButton ) 
    {             
        overTemperatureDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
    }
}

static void fireAlarmDeactivationUpdate()
{
    if ( sirenStateRead() ) 
    {
        if ( codeMatchFrom(CODE_KEYPAD) || codeMatchFrom(CODE_PC_SERIAL) ) 
        {
            fireAlarmDeactivate();
        }
    }
}

static void fireAlarmDeactivate()
{
    sirenStateWrite(OFF);
	strobeLightStateWrite(OFF);
    overTemperatureDetected = OFF;
    overHumidityDetected = OFF;    
}

static int fireAlarmStrobeTime()
{
    if( overTemperatureDetectedRead() && overHumidityDetectedRead() ) 
    {
        return STROBE_TIME_OVER_HUM_AND_OVER_TEMP;
        
    } else if ( overTemperatureDetectedRead() ) {
        
        return STROBE_TIME_OVER_TEMP;
        
    } else if ( overHumidityDetectedRead() ) {
        
        return STROBE_TIME_HUM;
         
    }else {
        
        return 0;
    }
}
