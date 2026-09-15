#include "Sensor.h"

Sensor::Sensor(uint8_t t, uint8_t e)
: trig(t), echo(e), threshold(30) {}
void Sensor::begin(){
    pinMode(trig, OUTPUT);
    pinMode(echo,  INPUT);
}
bool Sensor::near(){
    digitalWrite(trig,LOW );
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW );
    distance = pulseIn(echo,1,20000) * 0.0343 / 2;
    if(distance == 0){ return false; }
    return (distance < threshold);
}
