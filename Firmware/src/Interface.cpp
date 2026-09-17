#include "Interface.h"

Interface::Interface(Display& dis,Joystick& joy)
: display(dis), joystick(joy) {}
void Interface::begin(){
    display.begin();
    joystick.begin();
}
void Interface::select(){
    for(;;){
        joystick.x((uint8_t)joystick.y());
        display.setStatus(joystick.getstation(0),joystick.getstation(1));
        display.menu();
        if( joystick.pressed() ) {
            display.selected();
            delay(200); 
            break;
        }
    }
}
uint8_t Interface::getstation(uint8_t i){ return joystick.getstation(i); }
void Interface::init(){ display.init(); }
