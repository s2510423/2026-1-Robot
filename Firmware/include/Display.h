#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

class Display{
    private:
        LiquidCrystal_I2C display;
        uint8_t departure;
        uint8_t arrival;
    public:
        Display();
        void printCode(uint8_t code);
        void begin();
        void menu();
        void setStatus(uint8_t dep, uint8_t arr);
        void selected();
};


/*
1234567890123456
----------------
 [Departure] TR 
 [ Arrival ] 01
----------------
  [ Selected ]  
  TR  ===>  01  
*/