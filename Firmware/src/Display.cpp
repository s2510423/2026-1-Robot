#include "Display.h"

Display::Display()
: display(0x27, 16, 2), classNumber(0), departure(true) {}
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
    display.setCursor(4,0);
    if(!departure)     { display.print(" Arrival "); }
    else if(departure) { display.print("Departure"); }
    display.setCursor(11,1);
    if (classNumber < 2) { display.print("0");}
    display.print(classNumber+8);
    display.print("  ");
}
void Display::setStatus(uint8_t num, bool dep) {
    classNumber = constrain(num,0,3);
    departure = dep;
}
void Display::init() {
    display.init();
    display.backlight();
    display.setCursor(4,1);
    display.print("Class ");
    display.setCursor(3,0);
    display.print("[         ]");
}
void Display::selected() {
    display.setCursor(4,0);
    display.print("Selected ");        
}
