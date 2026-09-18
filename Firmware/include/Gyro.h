#pragma once
#include <Arduino.h>
#include <Adafruit_MPU6050.h>

class Gyro{
    private: 
        sensors_event_t a,g,temp;
        unsigned long time;
        float angle, angleOffset;
        Adafruit_MPU6050 mpu;
    public:
        Gyro();
        void begin();
        float measure();
        void setAngle(float newAngle);
        float getAngle();
        void end();
};