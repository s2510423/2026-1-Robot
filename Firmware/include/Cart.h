#pragma once
#include <Arduino.h>
#include "Mobile.h"
#include "Radar.h"
#include "Gyro.h"
#include "Husky.h"
#include "Interface.h"

class Cart{
    private:
        Mobile& mobile;
        Radar& radar;
        Gyro& gyro;
        Husky& husky;
        Interface& interface;
    public:
        Cart(Mobile& m, Radar& r, Gyro& g, Husky& h, Interface& i);
        void begin();
        void straight();
        void turn(bool right);
        void drive();
};
