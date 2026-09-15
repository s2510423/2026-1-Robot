#include "Joystick.h"

Joystick::Joystick(uint8_t x, uint8_t y, uint8_t button)
: xPin(x), yPin(y), buttonPin(button), classNumber(0), departure(true) {}
void Joystick::begin(){
    pinMode(xPin,INPUT);
    pinMode(yPin,INPUT);
    pinMode(buttonPin,INPUT_PULLUP);
}
uint8_t Joystick::y(){
    unsigned long currentTime = micros();
    yValue = analogRead(yPin);
    if(yValue < 200 && !yMoved) {
        classNumber--;
        if(classNumber < 0){ classNumber = 3; }
        yMoved = true;
        time = currentTime;
    } 
    else if(yValue > 800 && !yMoved) {
        classNumber++;
        if(classNumber > 3){ classNumber = 0; }
        yMoved = true;
        time = currentTime;
    } 
    else if (
        yValue >= 200 && yValue <= 800 && yMoved
        && currentTime - time >= 300000
    ) { yMoved = false; }
    return classNumber;
}
bool Joystick::x(){
    unsigned long currentTime = micros();
    xValue = analogRead(xPin);
    if((xValue < 200 || xValue > 800) && !xMoved) {
        departure = !departure;
        xMoved = true;
        time = currentTime;
    }
    else if (
        xValue >= 200 && xValue <= 800 && xMoved
        && currentTime - time >= 300000
    ) { xMoved = false; }
    return departure;
}
bool Joystick::pressed(){ 
    if (digitalRead(buttonPin) == LOW){
        delay(10);
        if (digitalRead(buttonPin) == LOW){ return true;  }
        }
    else { return false; } 
}
