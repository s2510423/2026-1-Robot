#pragma once
#include <Arduino.h>
#include <Servo.h>

class SensorServo{
    private:
        Servo sv;
        uint8_t pin;
        unsigned long time;
        unsigned long servoDelay;
        uint8_t angle, minimum, maximum;
        int8_t deltaAngle;
    public:
        SensorServo(uint8_t p);
        void begin();
        void run(bool on);
        void setAngle(uint8_t a);
        void setMinMax(uint8_t Min, uint8_t Max);
        void end();
};
