#pragma once
#include <Arduino.h>


class Sensor{
    private:
        uint8_t trig, echo;
        float distance, threshold;
    public:
        Sensor(uint8_t t, uint8_t e);
        void begin();
        bool near();
        void end();
};
