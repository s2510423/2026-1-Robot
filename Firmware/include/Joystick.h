#pragma once
#include <Arduino.h>

class Joystick{
    private:
        uint8_t xPin, yPin, buttonPin;
        unsigned long time;
        bool xMoved, yMoved;
        uint16_t xValue, yValue;        
        uint8_t station[2];
        bool departure;
    public:
        Joystick(uint8_t x, uint8_t y, uint8_t button);
        void begin();
        uint8_t x(uint8_t i);
        bool y();
        bool pressed();
        uint8_t getstation(uint8_t i);
        void end();
};
