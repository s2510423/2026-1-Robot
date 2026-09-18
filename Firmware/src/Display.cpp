#include "Display.h"

Display::Display()
: display(0x27, 16, 2), departure(0), arrival(1) {}
void Display::begin(){
    Wire.begin();
    display.init();
    display.backlight();
    display.setCursor(0,0);
    display.print(" [Departure] ");
    display.setCursor(0,1);
    display.print(" [ Arrival ] ");
}
void Display::printCode(uint8_t code){
    if (code == 0) {display.print("TR"); }
    else {display.print(code+6); }
}
void Display::menu() {
    display.setCursor(13,0);
    printCode(departure);
    display.setCursor(13,1);
    printCode(arrival);
}
void Display::setStatus(uint8_t dep, uint8_t arr) {
    departure = constrain(dep,0,5);
    arrival = constrain(arr,0,5);
}
void Display::selected() {
    display.clear();
    display.setCursor(0,0);
    display.print("  [ Selected ]  ");
    display.setCursor(0,0);
    display.print("  ");
    printCode(departure);
    display.print("  ===>  ");
    printCode(arrival);
}
