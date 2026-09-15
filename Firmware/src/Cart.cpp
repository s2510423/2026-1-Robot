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
void Cart::straight(){
    float dead = 3.0;
    mobile.on();
    while (!husky.arrived()){
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
void Cart::turn(bool right){
    mobile.direction((uint8_t)right + 1);
    gyro.setAngle(0.0);
    int8_t pm = (uint8_t)right * 2 - 1 ;
    while ( (gyro.measure() < pm * 90.0) ^ !right) {
        if (radar.near()) { mobile.off(); }
        else { mobile.on(); }
    }
}
void Cart::drive(){
    static const int route[2][4][8] = {
        {   // arrival  : classroom
            {0,1,0,2,2,3,3,3},  // from office to class  8
            {0,1,0,0,2,2,3,3},  // from office to class  9
            {0,1,0,0,0,2,2,3},  // from office to class 10
            {0,1,0,0,0,0,2,2}   // from office to class 11
        },   
        {   // departure: classroom
            {0,2,0,1,1,3,3,3},  // from class 8  to office  
            {0,0,2,0,1,1,3,3},  // from class 9  to office
            {0,0,0,2,0,1,1,3},  // from class 10 to office
            {0,0,0,0,2,0,1,1}   // from class 11 to office
        } 
    };
    interface.select();
    for(uint8_t i = 0; i<8;i++){
        uint8_t j = route[interface.getDeparture()][interface.getClassNumber()][i];
        if (j==0){ straight(); }
        else if (j>0 && j<3){ turn((bool)j - 1); }
        else if (j==3){ mobile.off(); }
    }

}
