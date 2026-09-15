#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Display{
    private:
        LiquidCrystal_I2C display;
        uint8_t classNumber;
        bool departure;
    public:
        Display();
        void begin();
        void menu();
        void setStatus(uint8_t num, bool dep);
        void init();
        void selected();
};