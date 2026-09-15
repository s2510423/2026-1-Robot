#include "Husky.h"

void Husky::begin() { while(!lns.begin(Serial2)) { delay(100); } }
bool Husky::arrived() {
    HUSKYLENSResult result = lns.read();
    if( result.ID == 1 && (result.xCenter < 300 && result.xCenter > 60) && (result.yCenter < 200 && result.yCenter > 40)){ return true; }
    else{ return false; }
}