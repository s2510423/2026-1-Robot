#include "Cart.h"

Cart::Cart(Mobile& m, Radar& r, Gyro& g, Husky& h, Interface& i)
: mobile(m), radar(r), gyro(g), husky(h), interface(i) {}
void Cart::begin(){
    mobile.begin();
    radar.begin();
    gyro.begin();
    husky.begin();
    interface.begin();
}
void Cart::straight(uint8_t num){
    float dead = 3.0;
    mobile.on();
    while (!husky.arrived(num)){
        while( radar.near() ){ delay(100); } 
        float angle = gyro.measure();
        if( angle < -1*dead || angle > dead){
            uint8_t left = mobile.getLeft();
            uint8_t right = mobile.getRight();
            if(angle<0){
                if(left <=245){ mobile.mod(10,0); }
                else if(right>=10){ mobile.mod(0,-10); }
            }
            else if(angle>0){
                if(right<=245){ mobile.mod(0,10); }
                else if(left>=10){ mobile.mod(-10,0); }
            }
        }
    }
}
void Cart::turn(uint8_t dir){
    mobile.direction(dir);
    gyro.setAngle(0.0);
    int8_t pm = dir * 2 - 3;
    while ( (gyro.measure() < pm * 90.0) ^ !(bool)(dir-1)) {
        if (radar.near()) { mobile.off(); }
        else { mobile.on(); }
    }
} 
void Cart::drive(){
    static const uint8_t stations[6][2] = {
        {3,0}, {2,1}, {0,0}, {0,0}, {0,0}, {0,3}
    };
    interface.select();
    uint8_t dep = interface.getstation(0);
    uint8_t arr = interface.getstation(1);
    
    for(uint8_t i = 0; i<(dep-arr);i+=( (uint8_t)(dep>arr)*2  -1)){
        uint8_t j = stations[dep+i][(uint8_t)(dep>=arr)];
        if (j==0){ straight(0); }
        else if (j>0 && j<3){ turn((bool)j - 1); }
        else if (j==3){ mobile.off(); }
    }
}
