#include "mbed.h"
#include "arm_book_lib.h"
#include "temp_hum_sensor.h"
#include "DHT.h"

DHT sensor(D10,DHT11);

float tempSensorReadCelsius(){
    sensor.readData();
    return sensor.ReadTemperature(CELCIUS);
}

float humSensorRead(){
    sensor.readData();
    return sensor.ReadHumidity();
}