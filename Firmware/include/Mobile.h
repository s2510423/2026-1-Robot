#pragma once
#include <Arduino.h>
#include "Motor.h"

class Mobile{
    private:
        Motors& left; 
        Motors& right;
    public:
        Mobile(Motors& leftMotors, Motors& rightMotors);
        void begin();
        void on();
        void set(uint8_t l=255, uint8_t r=255);
        void mod(int16_t deltaL, int16_t deltaR);
        void off();
        void direction(uint8_t dir);
        uint8_t getLeft();
        uint8_t getRight();        
};
