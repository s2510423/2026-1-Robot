#pragma once
#include <Arduino.h>

class Motor{
    private: 
        uint8_t in1Pin, in2Pin, pwmPin;
        uint8_t output;
    public:
        Motor(uint8_t in1, uint8_t in2, uint8_t pwm);
        void begin();
        void direction(uint8_t dir);
        void on();
        void set(uint8_t out);
        void mod(int16_t deltaOut);
        uint8_t getOutput();
        void off();
};
struct Motors{ 
    Motor& Front; 
    Motor& Back; 
};