#include "Display.h"

Display::Display()
: display(0x27, 16, 2), departure(0), arrival(1) {}
void Display::begin(){
    Wire.begin();
    display.init();
    display.backlight();
    display.setCursor(4,1);
    display.print("Class ");
    display.setCursor(3,0);
    display.print("[         ]");
}
void Display::menu() {
    display.setCursor(13,0);
    if (departure==0){display.print("TR"); }
    display.setCursor(13,1);
    if (arrival==0){display.print("TR"); }
}
void Display::setStatus(uint8_t dep, uint8_t arr) {
    departure = constrain(dep,0,5);
    arrival = constrain(arr,0,5);
}
void Display::init() {
    display.init();
    display.backlight();
    display.setCursor(0,0);
    display.print(" [Departure] ");
    display.setCursor(0,1);
    display.print(" [ Arrival ] ");
}
void Display::selected() {
    display.clear();
    display.setCursor(0,0);
    display.print("  [ Selected ]  ");
    display.setCursor(0,0);
    display.print("  TR  ===>  01  ");       
}
