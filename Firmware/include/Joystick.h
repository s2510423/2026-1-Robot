#pragma once
#include <Arduino.h>

class Joystick{
    private:
        uint8_t xPin, yPin, buttonPin;
        unsigned long time;
        bool xMoved, yMoved;
        uint16_t xValue, yValue;
    public:
        Joystick(uint8_t x, uint8_t y, uint8_t button);
        void begin();
        uint8_t classNumber;
        bool departure;
        void y();
        void x();
        bool pressed();
};
