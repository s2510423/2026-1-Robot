#pragma once
#include <Arduino.h>
#include "Joystick.h"
#include "Display.h"

class Interface{
    private:
        Display& display;
        Joystick& joystick;
    public:
        Interface(Display& dis,Joystick& joy);
        void begin();
        void select();
        uint8_t getstation(uint8_t i);
        void init();
};
