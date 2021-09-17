//=====[#include guards - begin]===============================================

#ifndef _FIRE_ALARM_H_
#define _FIRE_ALARM_H_

//=====[Libraries]=============================================================

//=====[Declaration of public defines]=======================================



//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void fireAlarmInit();
void fireAlarmUpdate();
bool gasDetectorStateRead();
bool overTemperatureDetectorStateRead();
bool overHumidityDetectorStateRead();
bool gasDetectedRead();
bool overTemperatureDetectedRead();
bool overHumidityDetectedRead();

//=====[#include guards - end]=================================================

#endif // _FIRE_ALARM_H_