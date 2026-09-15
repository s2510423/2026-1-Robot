#pragma once
#include "HUSKYLENS.h"

class Husky{
    private:
        HUSKYLENS lns;
        HUSKYLENSResult result;
    public:
        void begin();
        bool arrived();
};