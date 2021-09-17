#include "arm_book_lib.h"
#include "mbed.h"
#include "servo_motor.h"

PwmOut servo(PB_4);

static void setPeriod( float period );

void servoControlInit()
{
    setPeriod( 0.020 );
    
    setDutyCycle( 0.0011 );
}
 
void setDutyCycle( float dutyCycle )
{
    servo.pulsewidth(dutyCycle);
}
 
//=====[Implementations of private functions]==================================
 
static void setPeriod( float period )
{
    servo.period(period);
}

