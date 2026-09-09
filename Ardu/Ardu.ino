// Libraries 
    // for DISPLAY
        #include <Wire.h>
        #include <LiquidCrystal_I2C.h>
    // for GYRO SENSOR
        #include <Adafruit_MPU6050.h>
        #include <Adafruit_Sensor.h>
    // for DFRobot HUSKYLENS
        #include "HUSKYLENS.h"
    // for servo motor
        #include <Servo.h>
// Define HUSKYLENS, DISPLAY, SENSOR
    HUSKYLENS lns;
    LiquidCrystal_I2C display(0x27, 16, 2);
    Adafruit_MPU6050 mpu;
    Servo sv;

// classes

class Motor{
    private: 
        uint8_t in1Pin, in2Pin, pwmPin;
        uint8_t output;
    public:
        Motor(uint8_t in1, uint8_t in2, uint8_t pwm)
        : in1Pin(in1), in2Pin(in2), pwmPin(pwm), output(255) {
            pinMode(in1, OUTPUT);
            pinMode(in2, OUTPUT);
            pinMode(pwm, OUTPUT);
        }
        void direction(uint8_t dir) { // 0: stop // 1: forward // 2: backward //
            static const uint8_t digit[3][2] = { {HIGH,HIGH}, {LOW,HIGH}, {HIGH,LOW} }; 
            digitalWrite(in1Pin, digit[dir][0]);
            digitalWrite(in2Pin, digit[dir][1]);
        }
        void on() { analogWrite(pwmPin, output); }
        void on(uint8_t out) { 
            output = out;
            on();
        }
        void mod(int16_t deltaOut) { 
            int16_t nextOut = (int16_t)output + deltaOut;
            output = constrain(nextOut, 0, 255);
            on();
        }
        void off() { analogWrite(pwmPin, 0); }
};
struct Motors{ Motor Front, Back; };
class Mobile{
    private:
        Motors left, right;
    public:
        Mobile(Motors leftMotors, Motors rightMotors)
        : left(leftMotors), right(rightMotors) {}
        void on(uint8_t l=255, uint8_t r=255) {
            left.Front.on(l);
            left.Back.on(l);
            right.Front.on(r);
            right.Back.on(r);
        }
        void mod(int16_t deltaL, int16_t deltaR){
            left.Front.mod(deltaL);
            left.Back.mod(deltaL);
            right.Front.mod(deltaR);
            right.Back.mod(deltaR);
        }
        void off(){
            left.Front.off();
            left.Back.off();
            right.Front.off();
            right.Back.off();
        }
};
class SensorServo{
    private:
        uint8_t pin;
        unsigned long time;
        unsigned long seroDelay;
        uint8_t angle, min, max;
        int8_t deltaAngle;
        Servo sv;
    public:
        SensorServo(uint8_t p)
        : pin(p), time(0), servoDelay(1500), angle(90), min(10), max(170), deltaAngle(2)
        { sv.attach(pin); }
        void run(bool on){
            if(on){
                unsigned long currentTime = micros();
                if ( currentTime - time >= servoDelay ){
                    if (angle <=min || angle >= max){ deltaAngle*=-1;}
                    angle += deltaAngle;
                    angle = constrain(angle, min, max);
                    sv.write(angle);
                    time = currentTime;
                }
            }  
        }
        void setAngle(uint8_t a) {
            angle = constrain(a, 0, 180);
            sv.write(angle);
        }
        void setMinMax(uint8_t Min, uint8_t Max) {
            min = constrain(Min, 0, 180);
            max = constrain(Max, 0, 180);
        }
};

class Sensor{
    private:
        uint8_t trig, echo;
        float distance, threshold;
    public:
        Sensor(uint8_t t, uint8_t e)
        : trig(t), echo(e), threshold(30) {
            pinMode(trig, OUTPUT);
            pinMode(echo,  INPUT);
        }
        bool near(){
            digitalWrite(trig,LOW );
            delayMicroseconds(2);
            digitalWrite(trig,HIGH);
            delayMicroseconds(10);
            digitalWrite(trig,LOW );
            distance = pulseIn(echo,1,20000) * 0.0343 / 2;
            if(distance == 0){ return false; }
            return (distance < threshold);
        }
};
class Gyro{
    private: 
        sensors_event_t a,g,temp;
        unsigned long time;
        float angle, angleOffset;
        Adafruit_MPU6050 mpu;
    public:
        Gyro()
        : angle(0) {
            mpu.begin();
            mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // accelerometer ±8g
            mpu.setGyroRange(MPU6050_RANGE_500_DEG);      // gyro ±500 deg/s
            mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);   // noise filter
            for (int i = 0; i < 500; i++) {
                mpu.getEvent(&a, &g, &temp);
                angleOffset += g.gyro.z;
                delay(2);
            }
            angleOffset *= 0.002;
            time = micros();
        }
        void measure() {
            mpu.getEvent(&a, &g, &temp);
            float deltaAngle =  g.gyro.z;
            unsigned long currentTime = micros();
            float dt = (currentTime - time ) / 100000.0;
            time = currentTime;
            angle += (deltaAngle - angleOffset) * (180.0 / PI) * dt;
            angle = constrain(angle, 0.0, 180.0);
        }
        void setAngle(float newAngle){ angle = constrain(newAngle, 0.0, 180.0); }
};
class Display{
    private:
        LiquidCrystal_I2C display;
        uint8_t classNumber;
        bool departure;
    public:
        Display()
        : display(0x27, 16, 2) {
            Wire.begin();
            display.init();
            display.backlight();
            display.setCursor(4,1);
            display.print("Class ");
            display.setCursor(3,0);
            display.print("[         ]");
        }
        void menu() {
            display.setCursor(4,0);
            if(!departure)     { display.print(" Arrival "); }
            else if(departure) { display.print("Departure"); }
            display.setCursor(11,1);
            if (classNumber < 10) { display.print("0");}
            display.print(classNumber);
            display.print("  ");
        }
        void setStatus(uint8_t num, bool dep) {
            classNumber = constrain(num,8,11);
            departure = dep;
        }
};
class Joystick{};
class Interface{};
class Husky{};

class Cart{};

// constants
    // array of direction codes of routes between teachers' office and each classrooms
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
    // array of pin numbers of motors
        const int motor[2][2][3] = {   // {IN1, IN2, PWM}
            { {6 , 7 , 2 }, /* front left */ {10, 11, 4 } /* front right */ },  
            { {8 , 9 , 3 }, /* back  lect */ {12, 13, 5 } /* back  right */ }   
        };
    
    const int sensor[3] = {23/*echo*/, 22/*trig*/, 44/*servo*/}; // sensor pin array
    const float threshold = 60.0; //threshold of distance: in function ( bool near() )
    const int controller[3] = {A1/*right-left*/, A0/*up-down*/, 14/*button*/}; // joystick pin arrray


// variables
    int servoAngle[2] = {60,-2}; // for function ( void sensorServo() )
    bool moved[2] = {false, false}; // represents whether joystick is moved in function ( void select() ), ( void departure() )
    int finish = 0;
    int selected[2] = {0,0}; // selected index of const int route[2][4][6]
    float angle = 0.0, angleOffset = 0.0; // measured with gyro seneor in function ( void gyro() )
    unsigned long time[7] = {0,0,0,0,0,0,0}; // for integration while measuring ( float angle ) in function ( void gyro() )
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
    for (int i=0; i<=1; i++) { for (int j=0; j<2; j++) { for (int k=0; k<2; k++) { digitalWrite(motor[i][j][k], hl[dircode[dir][i]][k]);} }; }
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
        analogWrite(motor[i][0][0], left); 
        analogWrite(motor[i][1][0], right); 
    }
}
bool near() {
    /* 
    measure distance between mobile and other object and returns wheter their distance is lower than threshold or not

        parameters: no parameter.

        constants: 
            const int sensor[3]
            const float threshold

        variables: 
            local : float distance: in centemeter unit
            global: no global variable.

        used in:
            void dirve(int dir)
    */
    time[0] = micros();
    unsigned long measurementDelay = 0;
    if (time[0] - time[6] > measurementDelay){
        digitalWrite(sensor[1],LOW );
        delayMicroseconds(2);
        digitalWrite(sensor[1],HIGH);
        delayMicroseconds(10);
        digitalWrite(sensor[1],LOW );
        float distance = pulseIn(sensor[0],1,20000) * 0.0343 / 2;
        Serial.println(distance);
        time[6] = micros();
        if(distance == 0){ return false; }
        return (distance < threshold);
    }
    else{return false;}
}
void sensorServo(){
    /* 
    move servo motor constantly for wide range of distance measurement

        parameters: no parameter

        constants: const int 

        variables:
            local : unsigned long delay
            global: 
                unsigned long time[4]
                bool servoMoved
        used in: void drive(int dir)
    */
    unsigned long servoDelay = 1500;
    time[0] = micros();
    if ( time[0] - time[5] >= servoDelay ){
        if (servoAngle[0] <=10 || servoAngle[0] >= 170){ servoAngle[1]*=-1;}
        servoAngle[0] += servoAngle[1];
        sv.write(servoAngle[0]);
        time[5] = time[0];
    //    Serial.println(servoAngle[0]);
    }
}
bool arrived() {
    /* 
    check if there is tag seen in the sight of HUSKYLENS

        parameters: no parameter

        constants: no constant

        variables:
            local : 
                HUSKYLENSResult result: information given by HUSKYLENS 
                unsigned long huskyDelay: delay of huskylens
            global: unsigned long time[4]

        used in: void drive(int dir)
    */

    HUSKYLENSResult result = lns.read(); 
    if( result.ID == 1 && (result.xCenter < 300 || result.xCenter > 60) && (result.yCenter < 200 || result.yCenter > 40)){ return true; }
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
                int time[4]
                float angleOffset, angle

        used in: void drive(int dir)
    */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float deltAngle =  g.gyro.z;
    time[0] = micros();
    float dt = (time[0] - time[1] ) / 100000.0;
    time[1] = time[0];
    angle += (deltAngle - angleOffset) * (180.0 / PI) * dt;
    //Serial.println(angle);
}
void straight(int right, int left){
    float dead = 3.0;
    gyro();
    sensorServo();
    if( near() ){
        time[0] = micros();
        while( near() ){direction(3);} 
        time[3] = micros();
        time[2] += time[3]-time[0];
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
        direction(0); 
        motorOn(left, right);
    }
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
                int time[4]
                float angle

        used in: void loop()
    */

    // initialize main variables
    int left = 255, right = 255;
    angle = 0.0;
    for(int i=0;i<2;i++){ time[i] = micros(); }
    // initialize motor output
    direction(dir);
    motorOn(left, right);
    // rotation
    if     ( 1 <= dir && dir <= 2) {  
        Serial.println("rotation...");
        for(;abs(angle)<90;gyro()){ direction(dir); }  
        }
    //straight
    else if( dir == 0){
        Serial.println("Going straight...");
        for(;arrived() && finish == 1;){
            straight(left,right);
        }
        finish = 0;
        for(;finish != 1;){
            straight(left,right);
            if(arrived()){break;}
        }
        finish = 1;
    }
    angle = 0.0;
    direction(3);
    delay(100);
    motorOn(0,0);
    Serial.println("Continue...");
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
                bool moved[2]
                int selected[2]

        used in: void select()
    */
    time[0] = micros();
    int value = analogRead(controller[1]);
    if(value < 100 && !moved[1]) {
        selected[1]--;
        if(selected[1] < 0){ selected[1] = 3; }
        moved[1] = true;
        moved[0] = true;
        time[4] = micros();
    } 
    else if(value > 900 && !moved[1]) {
        selected[1]++;
        if(selected[1] > 3){ selected[1] = 0; }
        moved[1] = true;
        moved[0] = true;
        time[4] = micros();
    } 
    else if(value >= 100 && value <= 900 && time[0] - time[4] >= 300000) { moved[1] = false; }
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
                int selected[]
                
        used in: void select()
    */
    time[0] = micros();
    int value = analogRead(controller[0]);
    if((value < 100 || value > 900) && !moved[0]) {
        if(selected[0] == 0){ selected[0] = 1; }
        else if(selected[0] == 1){ selected[0] = 0; }
        moved[0] = true;
        moved[1] = true;
        time[4] = micros();
    }
    else if(value >= 100 && value <= 900 && time[0] - time[4] >= 300000) { moved[0] = false; }
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
    //                                                          |   [Departure]  |
    //                                                          |    Class 00    |
    display.setCursor(4,0);
    if(selected[0] == 0)     { display.print(" Arrival "); }
    else if(selected[0] == 1){ display.print("Departure"); }
    display.setCursor(11,1);
    if (selected[1] <= 1) { display.print("0");}
    display.print(8+selected[1]);
    display.print("  ");
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
            display.setCursor(4,0);
            display.print("Selected ");
            delay(200); 
            break;
        }
    }
}
void setup(){
    // begin connections
    Serial.begin(9600);
    Serial2.begin(9600);
    
    while(!lns.begin(Serial2)){delay(100);};
    Wire.begin();
    display.init();
    display.backlight();
    display.setCursor(4,1);
    display.print("Class ");
    display.setCursor(3,0);
    display.print("[         ]");
    mpu.begin();


    // range of gyro sensor measurment
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
    sv.attach(sensor[2]);
    int io[2] = {INPUT, OUTPUT};
    pinMode(controller[2], INPUT_PULLUP);
    for( int i=0; i<2; i++){
        pinMode(sensor[i],io[i]);
        for( int j=0; j<2; j++){ for (int k=0; k<3; k++){ pinMode(motor[i][j][k], OUTPUT);}}
        }
    
    }
void loop(){     
    // selection -> route based driving loop
    select();
    drive(0);
    //for( int i=0; i<8; i++){ drive(route[selected[0]][selected[1]][i]); }
}