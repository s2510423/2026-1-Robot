// Libraries for DISPLAY
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// for GYRO SENSOR
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// for DFRobot HUSKYLENS
#include "HUSKYLENS.h"

// Define HUSKYLENS, DISPLAY, SENSOR
HUSKYLENS lns;
Adafruit_SSD1306 display(128, 64, &Wire);
Adafruit_MPU6050 mpu;


/* array of direction codes of routes between teachers' office and each classrooms */
const int route[2][4][8] = {
    {   // departure from teachers' office
        {0,1,0,2,2,3,3,3},  // from office to class  8
        {0,1,0,0,2,2,3,3},  // from office to class  9
        {0,1,0,0,0,2,2,3},  // from office to class 10
        {0,1,0,0,0,0,2,2}   // from office to class 11
    },   
    {   // departure from classrooms
        {0,2,0,1,1,3,3,3},  // from class 8  to office
        {0,0,2,0,1,1,3,3},  // from class 9  to office
        {0,0,0,2,0,1,1,3},  // from class 10 to office
        {0,0,0,0,2,0,1,1}   // from class 11 to office
    }    
};

/* array of pin numbers of motors */
const int motor[2][2][3] = {   // {direction1, direction2, PWM}
    { {6 , 7 , 2 }, /* front left */ {10, 11, 4 } /* front right */ },  
    { {8 , 9 , 3 }, /* back  lect */ {12, 13, 5 } /* back  right */ }   
};

const int sensor[2] = {22/*echo*/, 23/*trig*/}; // sensor pin array
const float threshold = 30.0; //threshold of distance: in function ( bool near() )

const int controller[3] = {A0/*right-left*/, A1/*up-down*/, 14/*button*/}; // joystick pin arrray
bool moved[2] = {false, false}; // represents whether joystick is moved in function ( void select() ), ( void departure() )


int selected[2] = {0,0}; // selected index of const int route[2][4][6]

float angle = 0.0, angleOffset = 0.0; // measured with gyro seneor in function ( void gyro() )
unsigned long time[2] = {0,0}; // for integration while measuring ( float angle ) in function ( void gyro() )


/*[FORM OF DESCRIPTION OF FUNCTIONS]*/

/* 
description: 

    parameters: 

    constants:

    variables:
        local :
        global:

    used in:
*/




void direction(int dir) {
/*
control direction of the mobile

    parameters:
        int dir: direction. 
            0: forward
            1: left
            2: right
            3: stop
            4: backward

    constants: const int motor[2][2][3]

    variables:
        local :
            int hl[3][2]: direction of ONE SPECIFIC MOTOR
            int dircode[5][2]: direction of WHOLE MOBILE
        global: not used.

    used in: void drive(int dir)
*/
    int hl[3][2] = { {HIGH,HIGH}/*stop*/, {LOW,HIGH}/*forward*/, {HIGH,LOW}/*backward*/ };
    int dircode[5][2] = { {1,1}/*forward*/,{2,1}/*turn left*/,{1,2}/*turn right*/,{0,0}/*stop*/,{2,2}/*backward*/ };
    for (int i=0; i<=1; i++) { for (int j=0; j<2; j++) { for (int k=0; k<2; k++) { digitalWrite(motor[i][j][k], hl[dircode[dir]][j]);} }; }
} 


void motorOn(int left, int right){ 
/* 
turn on the motor with proper intensity

    parameters:
        int left : intensity of left  motors
        int right: intensity of right motors

    constants: const int motor[2][2][3]

    variables: not used.

    used in: void drive(int dir)
*/
    for (int i=0; i<2; i++) { 
        analogWrite(motor[0][i][2], left); 
        analogWrite(motor[1][i][2], right); 
    }
}


bool near() {
/* 
measure distance between mobile and other object and returns wheter their distance is lower than threshold or not

    parameters: no parameter.

    constants: 
        const int sensor[2]
        const float threshold

    variables: 
        local : float distance: in centemeter unit
        global: no global variable.

    used in:
        void dirve(int dir)
*/
    digitalWrite(sensor[1],LOW );
    delayMicroseconds(2);
    digitalWrite(sensor[1],HIGH);
    delayMicroseconds(10);
    digitalWrite(sensor[1],LOW );
    float distance = pulseIn(sensor[0],1,20000) * 0.0343 / 2.0;
    if(distance == 0){ return false;}
    return (distance < threshold);
}

bool arrived() {
/* 
check if there is tag seen in the sight of HUSKYLENS

    parameters: no parameter

    constants: no constant

    variables:
        local : HUSKYLENSResult result: information given by HUSKYLENS 
        global: no global variable

    used in: void drive(int dir)
*/
    HUSKYLENSResult result = lns.read()
    if(result.ID == 1 && (result.xCenter < 300 || result.xCenter > 60) && (result.yCenter < 200 || result.yCenter > 40)){ return true; }
    else{ return false; }
}

void gyro(){
/* 
read angle rotation data from gyro sensor

    parameters: no parameter

    constants: no constant

    variables:
        local :
            float deltAngle, dt
            sensors_event_t a, g, temp
        global: 
            int time[2]
            float angleOffset, angle

    used in: void drive(int dir)
*/
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float deltAngle =  g.gyro.z;
    time[0] = micros();
    float dt = (time[0] - time[1] ) / 1000000.0;
    for(int i=0;i<2;i++){ time[i] = micros(); }
    angle += (deltAngle - angleOffset) * (180.0 / PI) * dt;
}

void drive(int dir) { 
/* 
drive mobile by hard-coded route array

    parameters: 
        int dir: direction. 
            0: forward
            1: left
            2: right
            3: stop
            4: backward

    constants:
        const int motor[2][2][3]
        const int route[2][4][8]

    variables:
        local : 
            bool finish: monitor wheter movement should finish
            int left, right: intensity of function { void motorOn(int left, int right) }
            float dead: deadline of angle tolerance during going straight
        global:
            int time[2]
            float angle

    used in: void loop()
*/
    int left = 255, right = 255;
    angle = 0.0;
    float dead = 3.0;
    for(int i=0;i<2;i++){ time[i] = micros(); }

    direction(dir);
    motorOn(left, right);
    
    if     ( dir == 1) {  for(;angle< 90;gyro()){ direction(dir); }  }
    else if( dir == 2) {  for(;angle>-90;gyro()){ direction(dir); }  }
    else if( dir == 0){
        for(bool finish = false; !finish; finish = arrived()){
            gyro();
            if( near() ){
                direction(3);
                while (near()) { delay(100); } 
            }
            else if( angle < -1*dead || angle > dead){
                if(angle<0){
                    if(left<=245){ left+=10; }
                    else if(right>=10){ right-=10; }
                }
                else if(angle>0){
                    if(right<=245){ right+=10; }
                    else if(left>=10){ left-=10; }
                }
                direction(dir); 
                motorOn(left,right)
            }
        }
    }
    angle = 0.0;
    direction(3);
    delay(100);
    motorOn(0,0);
}

void control(){
/* 
joystick - up and down

    parameters: no parameter

    constants: const int controller[3]

    variables:
        local : 
            int value: analog read value from controller pin
        global: 
            bool moved
            int selected[2]
    used in: void select()
*/
    int value = analogRead(controller[1]);
    if(value < 300 && !moved) {
        selected[1]--;
        if(selected[1] < 0){ selected[1] = 3; }
        moved = true;
    } 
    else if(value > 700 && !moved) {
        selected[1]++;
        if(selected[1] > 2){ selected[1] = 0; }
        moved = true;
    } 
    else if(value >= 300 && value <= 700) { moved = false; }
}

void departure(){
/* 
joystick - left and right

    parameters: no parameter

    constants: const int controller[3]

    variables:
        local : 
            int value: analog read value from controller pin
        global: 
            bool moved
            int selected[2]
    used in: void select()
*/
    int value = analogRead(controller[0]);
    if((value < 300 || value > 700) && !moved) {
        if(selected[0] == 0){ selected[0] = 1; }
        else if(selected[0] == 1){ selected[0] = 0; }
        moved = true;
    }
    else if(value >= 300 && value <= 700) { moved = false; }
}   

void menu(){
/* 
display selection menu

    parameters: no parameter

    constants: no constant

    variables:
        local : no local variable
        global: int selected[2]

    used in: void select()
*/
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 24);
    if(selected[0] == 0){ dislay.print("Arrival:        "); }
    else if(selected[0] == 1){ display.print("Departure from: "); }
    display.print("Class ");
    display.println(8+selected[1]);
    display.display();
}
void select(){
/* 
total user interface for route selection

    parameters: no parameter

    constants: const int controller[3]

    variables: no variable

    used in: void loop()
*/   
    while (true) {
        control();
        departure();
        menu();      
        if(digitalRead(controller[2]) == LOW) {
            delay(200); 
            break;
        }
    }
}

void setup(){
    // begin connections
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    mpu.begin();
    Serial2.begin(38400);
    while(!lns.begin(Serial2)){delay(100);};


    // range of sensor measurment
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // accelerometer ±8g
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);      // gyro ±500 deg/s
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);   // noise filter
    
    // gyro sensor initialization
    for (int i = 0; i < 500; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        angleOffset += g.gyro.z;
        delay(2);
    }
    angleOffset *= 0.002;
    time[1] = micros();
    
    // pin settings
    int io[] = {INPUT, OUTPUT};
    pinMode(controller[1], INPUT_PULLUP);
    for( int i=0; i<2; i++){
        pinMode(sensor[i],io[i]);
        for( int j=0; j<2; j++){ for (int k=1; k<=2; k++){ pinMode(motor[i][j][k], OUTPUT);}}
    }
}
void loop(){
// selection -> route based driving loop
    select();
    for( int i=0; i<6; i++){ drive(route[selected[0]][selected[1]][i]); }
}