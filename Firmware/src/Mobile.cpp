#include "Mobile.h"

Mobile::Mobile(Motors& leftMotors, Motors& rightMotors)
: left(leftMotors), right(rightMotors) {}
void Mobile::begin() {
    left.Front.begin();
    left.Back.begin();
    right.Front.begin();
    right.Back.begin();
}
void Mobile::on() {
    left.Front.on();
    left.Back.on();
    right.Front.on();
    right.Back.on();
}
void Mobile::set(uint8_t l, uint8_t r) {
    left.Front.set(l);
    left.Back.set(l);
    right.Front.set(r);
    right.Back.set(r);
}
void Mobile::mod(int16_t deltaL, int16_t deltaR){
    left.Front.mod(deltaL);
    left.Back.mod(deltaL);
    right.Front.mod(deltaR);
    right.Back.mod(deltaR);
}
void Mobile::off(){
    left.Front.off();
    left.Back.off();
    right.Front.off();
    right.Back.off();
}
void Mobile::end(){
    left.Front.end();
    left.Back.end();
    right.Front.end();
    right.Back.end();
}
void Mobile::direction(uint8_t dir) { // 0: forward // 1: left // 2: right // 3: stop
    static const uint8_t dirTable[4][2] = {{1,1},{2,1},{1,2},{0,0}};
    left.Front.direction (dirTable[dir][0]);
    left.Back.direction  (dirTable[dir][0]);
    right.Front.direction(dirTable[dir][1]);
    right.Back.direction (dirTable[dir][1]);
}
uint8_t Mobile::getLeft(){ return left.Front.getOutput(); }
uint8_t Mobile::getRight(){ return right.Front.getOutput(); }

