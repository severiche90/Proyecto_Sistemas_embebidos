//=====[Libraries]=============================================================
#include "arm_book_lib.h"
#include "mbed.h"
#include "hum_suelo_sensor.h"

//=====[Declaration of private defines]======================================

#define HL69_NUMBER_OF_AVG_SAMPLES 10

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

AnalogIn hl69(A2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

int hl69Humedad;
float hl69ReadingsArray[HL69_NUMBER_OF_AVG_SAMPLES];

//=====[Declarations (prototypes) of private functions]========================

static float map(float in, float inMin, float inMax, float outMin, float outMax);

//=====[Implementations of public functions]===================================

void humedadSensorInit()
{
    int i;
    
    for( i=0; i<HL69_NUMBER_OF_AVG_SAMPLES; i++ )
    {
        hl69ReadingsArray[i] = 0;
    }
}

void humedadSensorUpdate()
{
    static int hl69SampleIndex = 0;
    float hl69ReadingsSum = 0.0;
    float hl69ReadingsAverage = 0.0;

    int i = 0;
    
    hl69ReadingsArray[hl69SampleIndex] = hl69.read();

    hl69SampleIndex++;
    
    if (hl69SampleIndex >= HL69_NUMBER_OF_AVG_SAMPLES) 
    {
        hl69SampleIndex = 0;
    }
    
    hl69ReadingsSum = 0.0;
    
    for (i = 0; i < HL69_NUMBER_OF_AVG_SAMPLES; i++) 
    {
        hl69ReadingsSum = hl69ReadingsSum + hl69ReadingsArray[i];
    }
    
    hl69ReadingsAverage = hl69ReadingsSum / HL69_NUMBER_OF_AVG_SAMPLES;
    hl69ReadingsAverage = hl69ReadingsAverage*100;
    hl69Humedad = (int)map(hl69ReadingsAverage, 99.0, 22.0, 0.0, 100.0);
    
}  

int humedadSensorRead()
{
    return hl69Humedad; 
} 


//=====[Implementations of private functions]==================================
        
static float map(float in, float inMin, float inMax, float outMin, float outMax) {
  // check it's within the range
  if (inMin<inMax) { 
    if (in <= inMin) 
      return outMin;
    if (in >= inMax)
      return outMax;
  } else {  // cope with input range being backwards.
    if (in >= inMin) 
      return outMin;
    if (in <= inMax)
      return outMax;
  }
  // calculate how far into the range we are
  float scale = (in-inMin)/(inMax-inMin);
  // calculate the output.
  return outMin + scale*(outMax-outMin);
}