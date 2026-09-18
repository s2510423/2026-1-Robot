#include "SensorServo.h"

SensorServo::SensorServo(uint8_t p)
    : pin(p), time(0), servoDelay(1500), angle(90), minimum(10), maximum(170), deltaAngle(2){}
void SensorServo::begin(){ sv.attach(pin); }
void SensorServo::run(bool on){
    if(on){
        unsigned long currentTime = micros();
        if ( currentTime - time >= servoDelay ){
            if (angle <=minimum || angle >= maximum){ deltaAngle*=-1;}
            angle += deltaAngle;
            angle = constrain(angle, minimum, maximum);
            sv.write(angle);
            time = currentTime;
        }
    }  
}
void SensorServo::setAngle(uint8_t a) {
    angle = constrain(a, 0, 180);
    sv.write(angle);
}
void SensorServo::setMinMax(uint8_t Min, uint8_t Max) {
    minimum = constrain(Min, 0, 180);
    maximum = constrain(Max, 0, 180);
}
void SensorServo::end(){ sv.detach(); }
