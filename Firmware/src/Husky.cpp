#include "Husky.h"

void Husky::begin() { 
    Serial2.begin(9600);
    while(!lns.begin(Serial2)) { delay(100); }
}
bool Husky::arrived(uint8_t tagNum) {
    HUSKYLENSResult result = lns.read();
    if( result.ID == tagNum && (result.xCenter < 300 && result.xCenter > 60) && (result.yCenter < 200 && result.yCenter > 40)){ return true; }
    else{ return false; }
}
void Husky::end(){ Serial2.end(); }
