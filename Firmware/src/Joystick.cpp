#include "Joystick.h"

Joystick::Joystick(uint8_t x, uint8_t y, uint8_t button)
: xPin(x), yPin(y), buttonPin(button), departure(true) {
    station[0] = 0;
    station[1] = 1;
}
void Joystick::begin(){
    pinMode(xPin,INPUT);
    pinMode(yPin,INPUT);
    pinMode(buttonPin,INPUT_PULLUP);
}
uint8_t Joystick::x(uint8_t i){
    unsigned long currentTime = micros();
    xValue = analogRead(xPin);
    if(xValue < 200 && !xMoved) {
        station[i]--;
        if ((station[i] == 1)||(station[0] == station[1])){ station[i] --; }
        if(station[i] < 0){ station[i] = 5; }
        xMoved = true;
        time = currentTime;
    } 
    else if(xValue > 800 && !xMoved) {
        station[i]++;
        if ((station[i] == 1)||(station[0] == station[1])){ station[i] ++; }
        if(station[i] > 5){ station[i] = 0; }
        xMoved = true;
        time = currentTime;
    } 
    else if (currentTime - time >= 300000) { yMoved = false; }
    return station[i];
}
bool Joystick::y(){
    unsigned long currentTime = micros();
    yValue = analogRead(yPin);
    if((yValue < 200 || yValue > 800) && !yMoved) {
        departure = !departure;
        xMoved = true;
        time = currentTime;
    }
    else if ( currentTime - time >= 300000 ) { xMoved = false; }
    return departure;
}
bool Joystick::pressed(){ 
    if (digitalRead(buttonPin) == LOW){
        delay(10);
        if (digitalRead(buttonPin) == LOW){ return true;  }
    }
    return false;
}
uint8_t Joystick::getstation(uint8_t i){ return station[i]; }
