#include "Joystick.h"

Joystick::Joystick(uint8_t x, uint8_t y, uint8_t button)
: xPin(x), yPin(y), buttonPin(button) {}
void Joystick::begin(){
    pinMode(xPin,INPUT);
    pinMode(yPin,INPUT);
    pinMode(buttonPin,INPUT_PULLUP);
}
uint8_t classNumber = 0;
uint8_t departure = true;
void Joystick::y(){
    unsigned long currentTime = micros();
    yValue = analogRead(yPin);
    if(yValue < 100 && !yMoved) {
        classNumber--;
        if(classNumber < 0){ classNumber = 3; }
        yMoved = true;
        time = currentTime;
    } 
    else if(yValue > 900 && !yMoved) {
        classNumber++;
        if(classNumber > 3){ classNumber = 0; }
        yMoved = true;
        time = currentTime;
    } 
    else if (
        yValue >= 100 && yValue <= 900 && yMoved
        && currentTime - time >= 300000
    ) { yMoved = false; }
}
void Joystick::x(){
    unsigned long currentTime = micros();
    xValue = analogRead(xPin);
    if((xValue < 100 || xValue > 900) && !xMoved) {
        departure = !departure;
        xMoved = true;
        time = currentTime;
    }
    else if (
        xValue >= 100 && xValue <= 900 && !xMoved
        && currentTime - time >= 300000
    ) { xMoved = false; }
}
bool Joystick::pressed(){ 
    if (analogRead(buttonPin) == LOW){ return true; }
    else { return false; } 
}
