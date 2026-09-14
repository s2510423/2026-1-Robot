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
        : in1Pin(in1), in2Pin(in2), pwmPin(pwm), output(255) { }
        void begin() { 
            pinMode(in1Pin, OUTPUT);
            pinMode(in2Pin, OUTPUT);
            pinMode(pwmPin, OUTPUT); 
        }
        void direction(uint8_t dir) { // 0: stop // 1: forward // 2: backward //
            static const uint8_t digit[3][2] = { {HIGH,HIGH}, {LOW,HIGH}, {HIGH,LOW} }; 
            digitalWrite(in1Pin, digit[dir][0]);
            digitalWrite(in2Pin, digit[dir][1]);
        }
        void on() { analogWrite(pwmPin, output); }
        void set(uint8_t out) { 
            output = out;   
            on();
        }
        void mod(int16_t deltaOut) { 
            int16_t nextOut = (int16_t)output + deltaOut;
            output = constrain(nextOut, 0, 255);
            on();
        }
        uint8_t getOutput() { return output; }
        void off() { analogWrite(pwmPin, 0); }
};
struct Motors{ Motor& Front, Back; };
class Mobile{
    private:
        Motors& left, right;
    public:
        Mobile(Motors& leftMotors, Motors& rightMotors)
        : left(leftMotors), right(rightMotors) {}
        void begin() {
            left.Front.begin();
            left.Back.begin();
            right.Front.begin();
            right.Back.begin();
        }
        void on() {
            left.Front.on();
            left.Back.on();
            right.Front.on();
            right.Back.on();
        }
        void set(uint8_t l=255, uint8_t r=255) {
            left.Front.set(l);
            left.Back.set(l);
            right.Front.set(r);
            right.Back.set(r);
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
        void direction(uint8_t dir) { // 0: forward // 1: left // 2: right // 3: stop
            static const uint8_t dirTable[4][2] = {{1,1},{2,1},{1,2},{0,0}};
            left.Front.direction (dirTable[dir][0]);
            left.Back.direction  (dirTable[dir][0]);
            right.Front.direction(dirTable[dir][1]);
            right.Back.direction (dirTable[dir][1]);
        }
        uint8_t getLeft(){ return left.Front.getOutput(); }
        uint8_t getRight(){ return right.Front.getOutput(); }
        
};
class SensorServo{
    private:
        Servo sv;
        uint8_t pin;
        unsigned long time;
        unsigned long servoDelay;
        uint8_t angle, minimum, maximum;
        int8_t deltaAngle;
    public:
        SensorServo(uint8_t p)
        : pin(p), time(0), servoDelay(1500), angle(90), minimum(10), maximum(170), deltaAngle(2){}
        void begin(){ sv.attach(pin); }
        void run(bool on){
            if(on){
                unsigned long currentTime = micros();
                if ( currentTime - time >= servoDelay ){
                    if (angle <=minimum || angle >= maximum){ deltaAngle*=-1;}
                    angle += deltaAngle;
                    angle = constrain(angle, minimum, maximum);
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
            minimum = constrain(Min, 0, 180);
            maximum = constrain(Max, 0, 180);
        }
};
class Sensor{
    private:
        uint8_t trig, echo;
        float distance, threshold;
    public:
        Sensor(uint8_t t, uint8_t e)
        : trig(t), echo(e), threshold(30) {}
        void begin(){
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
class Radar{
    private:
        Sensor& sensor;
        SensorServo& sensorServo;
    public:
        Radar(Sensor& snsr, SensorServo& snsrvo)
        : sensor(snsr), sensorServo(snsrvo) {}
        void begin(){
            sensor.begin();
            sensorServo.begin();
        }
        bool near(){
            sensorServo.run(!sensor.near());
            return sensor.near();
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
        : angle(0), angleOffset(0) {}
        void begin() {
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
        float measure() {
            mpu.getEvent(&a, &g, &temp);
            float deltaAngle =  g.gyro.z;
            unsigned long currentTime = micros();
            float dt = (currentTime - time ) / 1000000.0;
            time = currentTime;
            angle += (deltaAngle - angleOffset) * (180.0 / PI) * dt;
            return angle;
        }
        void setAngle(float newAngle){ angle = newAngle; }
        float getAngle(){ return angle; }
};
class Display{
    private:
        LiquidCrystal_I2C display;
        uint8_t classNumber;
        bool departure;
    public:
        Display()
        : display(0x27, 16, 2), classNumber(8), departure(true) {}
        void begin(){
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
            if (classNumber < 3) { display.print("0");}
            display.print(classNumber+8);
            display.print("  ");
        }
        void setStatus(uint8_t num, bool dep) {
            classNumber = constrain(num,8,11);
            departure = dep;
        }
        void init() {
            display.init();
            display.backlight();
            display.setCursor(4,1);
            display.print("Class ");
            display.setCursor(3,0);
            display.print("[         ]");
        }
        void selected() {
            display.setCursor(4,0);
            display.print("Selected ");        
        }
};
class Joystick{
    private:
        uint8_t xPin, yPin, buttonPin;
        unsigned long time;
        bool xMoved, yMoved;
        uint16_t xValue, yValue;
    public:
        Joystick(uint8_t x, uint8_t y, uint8_t button)
        : xPin(x), yPin(y), buttonPin(button) {}
        void begin(){
            pinMode(xPin,INPUT);
            pinMode(yPin,INPUT);
            pinMode(buttonPin,INPUT_PULLUP);
        }
        uint8_t classNumber = 0;
        bool departure = true;
        void y(){
            unsigned long currentTime = micros();
            yValue = analogRead(yPin);
            if(yValue < 100 && !yMoved) {
                classNumber--;
                if(classNumber < 0){ classNumber = 3; }
                yMoved = true;
                time = currentTime;
            } 
            else if(yValue > 900 && !yMoved) {
                classNumber++;
                if(classNumber > 3){ classNumber = 0; }
                yMoved = true;
                time = currentTime;
            } 
            else if (
                yValue >= 100 && yValue <= 900 && yMoved
                && currentTime - time >= 300000
            ) { yMoved = false; }
        }
        void x(){
            unsigned long currentTime = micros();
            xValue = analogRead(xPin);
            if((xValue < 100 || xValue > 900) && !xMoved) {
                departure = !departure;
                xMoved = true;
                time = currentTime;
            }
            else if (
                xValue >= 100 && xValue <= 900 && !xMoved
                && currentTime - time >= 300000
            ) { xMoved = false; }
        }
        bool pressed(){ 
            if (analogRead(buttonPin) == LOW){ return true; }
            else { return false; } 
        }

};
class Interface{
    private:
        Display& display;
        Joystick& joystick;
    public:
        Interface(Display& dis,Joystick& joy)
        : display(dis), joystick(joy) {}
        void begin(){
            display.begin();
            joystick.begin();
        }
        void select(){
            for(;;){
                joystick.x();
                joystick.y();
                display.menu();      
                if( joystick.pressed() ) {
                    display.selected();
                    delay(200); 
                    break;
                }
            }
        }
        uint8_t getClassNumber(){ return joystick.classNumber; }
        uint8_t getDeparture(){ return joystick.departure; }
        void init(){ display.init(); }
};
class Husky{
    private:
        HUSKYLENS lns;
        HUSKYLENSResult result;
    public:
        void begin(){while(!lns.begin(Serial2)) { delay(100); }}
        bool arrived() {
            result = lns.read();
            if( result.ID == 1 && (result.xCenter < 300 || result.xCenter > 60) && (result.yCenter < 200 || result.yCenter > 40)){ return true; }
            else{ return false; }
        }
};

class Cart{
    private:
        Mobile& mobile;
        Radar& radar;
        Gyro& gyro;
        Interface& interface;
        Husky& husky;
    public:
        Cart(Mobile& m, Radar& r, Gyro& g, Husky& h, Interface& i)
        : mobile(m), radar(r), gyro(g), husky(h), interface(i) {}
        void begin(){
            mobile.begin();
            radar.begin();
            gyro.begin();
        }
        void straight(){
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
        void turn(bool right){
            mobile.direction((uint8_t)right + 1);
            gyro.setAngle(0.0);
            uint8_t pm = (uint8_t)right * 2 - 1 ;
            while ( (gyro.measure() < pm * 90.0) ^ !right) {
                if (radar.near()) { mobile.off(); }
                else { mobile.on(); }
            }
        }
        void drive(){
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
};
