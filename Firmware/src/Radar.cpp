#include "Radar.h"

Radar::Radar(Sensor& snsr, SensorServo& snsrvo)
: sensor(snsr), sensorServo(snsrvo) {}
void Radar::begin(){
    sensor.begin();
    sensorServo.begin();
}
bool Radar::near(){
    sensorServo.run(!sensor.near());
    return sensor.near();
}
void Radar::end(){
    sensor.end();
    sensorServo.end();
}
