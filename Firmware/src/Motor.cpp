#include "Motor.h"

Motor::Motor(uint8_t in1, uint8_t in2, uint8_t pwm)
: in1Pin(in1), in2Pin(in2), pwmPin(pwm), output(255) { }
void Motor::begin() { 
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(pwmPin, OUTPUT); 
}
void Motor::direction(uint8_t dir) { // 0: stop // 1: forward // 2: backward //
    static const uint8_t digit[3][2] = { {HIGH,HIGH}, {LOW,HIGH}, {HIGH,LOW} }; 
    digitalWrite(in1Pin, digit[dir][0]);
    digitalWrite(in2Pin, digit[dir][1]);
}
void Motor::on() { 
    begin();
    analogWrite(pwmPin, output); 
}
void Motor::set(uint8_t out) { 
    output = out;   
    on();
}
void Motor::mod(int16_t deltaOut) { 
    int16_t nextOut = (int16_t)output + deltaOut;
    output = constrain(nextOut, 0, 255);
    on();
}
uint8_t Motor::getOutput() { return output; }
void Motor::off() { analogWrite(pwmPin, 0); }
void Motor::end() { 
    pinMode(in1Pin, INPUT);
    pinMode(in2Pin, INPUT);
    pinMode(pwmPin, INPUT);  
}

