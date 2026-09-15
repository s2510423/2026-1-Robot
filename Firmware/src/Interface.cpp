#include "Interface.h"

Interface::Interface(Display& dis,Joystick& joy)
: display(dis), joystick(joy) {}
void Interface::begin(){
    display.begin();
    joystick.begin();
}
void Interface::select(){
    for(;;){
        display.setStatus(joystick.y(), joystick.x());
        display.menu();      
        if( joystick.pressed() ) {
            display.selected();
            delay(200); 
            break;
        }
    }
}
uint8_t Interface::getClassNumber(){ return joystick.y(); }
uint8_t Interface::getDeparture(){ return joystick.x(); }
void Interface::init(){ display.init(); }
