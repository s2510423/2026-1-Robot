#ifndef MOTOR_H
#define MOTOR_H
#include <Arduino.h>

class Motor {
    private:
        struct Pin{char pwm, fore, back;};
        const Pin pin;
    public:
        Motor(char p_pwm, char p_fore, char p_back);
        void drive(unsigned char speed, bool dir);
};
#endif