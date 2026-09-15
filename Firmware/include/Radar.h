#pragma once
#include <Arduino.h>
#include "Sensor.h"
#include "SensorServo.h"

class Radar{
    private:
        Sensor& sensor;
        SensorServo& sensorServo;
    public:
        Radar(Sensor& snsr, SensorServo& snsrvo);
        void begin();
        bool near();
};