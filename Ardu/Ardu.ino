#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_SSD1306 display(128, 64, &Wire);
Adafruit_MPU6050 mpu;

const unsigned long route[4][10]; //TODO: 실제경로 측정후 추가예정.
const int motor[2][2][3] = {   {  {2, 6, 7}, {4, 10, 11}  },  {  {3, 8, 9}, {5, 12, 13}  }   };
const int sensor[2] = {22, 23}, controller[] = {A1, 14};
const int thres = 30;

bool moved = false;
int selected = 0;
float angle = 0.0;
float angleOffset = 0.0;
unsigned long time[2];


void set(int dir, int pin) {
    int hl[3][2] = { {1,1}, {0,1}, {1,0} }; // 정지, 전진, 후진  
    for (int i=0; i<=1; i++) { for (int j=0; j<=1; j++) { digitalWrite(motor[pin][j][i+1], hl[dir][i]);} };
}

void direction(int dir) { // 0:직진, 1:좌, 2:우, 3:정지 4:후진
    int dircode[5][2] = { {1,1},{2,1},{1,2},{0,0},{2,2} };
    for (int i=0; i<=1; i++) { set(dircode[dir][i], i); }
} 
void motoron(int left, int right){ 
    for (int i=0; i<2; i++) { 
        analogWrite(motor[0][i][0], left); 
        analogWrite(motor[1][i][0], right); 
    }
}

bool near() {
    digitalWrite(sensor[1],0);
    delayMicroseconds(2);
    digitalWrite(sensor[1],1);
    delayMicroseconds(10);
    digitalWrite(sensor[1],0);
    unsigned long duration = pulseIn(sensor[0],1,20000);
    if (duration == 0) return false;
    float dist = duration * 0.0343 / 2.0;
    return (dist<thres);
}

bool arrived() { /* !!! HUSKYLENS LIBRARY AQUIRED !!! */ return false}

void gyro(){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float deltAngle =  g.gyro.z;
    time[0] = micros();
    float dt = (time[0] - time[1] ) / 1000000.0;
    time[1] = time[0];
    angle += (deltAngle - angleOffset) * (180.0 / PI) * dt;
}

void drive(int dir) {
    int left = 255, right = 255;
    angle = 0.0;
    float dead = 3.0;
    for(int i=0;i<2;i++){ time[i] = micros(); }

    direction(dir);
    motoron(left, right);
    
    if     ( dir == 1) {  for(;angle< 90;gyro()){ direction(dir); }  }
    else if( dir == 2) {  for(;angle>-90;gyro()){ direction(dir); }  }
    else{
        for(bool finish = false; !finish; motoron(left,right)){ 
            if( near() ){
                direction(3);
                while (near()) { delay(100); } 
            }
            else if(arrived()){ finish = true; }
            else{ 
                gyro();
                if( angle < -1*dead || angle > dead){
                    if(angle<0){
                        if(left<=245){ left+=10; }
                        else if (right>=10){ right-=10; }
                    }
                    else if(angle>0){
                        if(right<=245){ right+=10; }
                        else if (left>=10){ left-=10; }
                    }
                }
                direction(dir); 
            }
        }
    }
    angle = 0.0;
    direction(3);
    delay(100);
    motoron(0,0);
}
void control(){
    int Value = analogRead(controller[0]);
    if (Value < 300 && !moved) {
        selected--;
        if (selected < 0) selected = 3;
        moved = true;
    } 
    else if (Value > 700 && !moved) {
        selected++;
        if (selected > 2) selected = 0; 
        moved = true;
    } 
    else if (Value >= 300 && Value <= 700) { moved = false; }
}

void menu(){
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 24);
    display.print("Class ");
    display.println(8+selected);
    display.display();
}
int select(){
    
    while (true) {
        control();
        menu();      
        if (digitalRead(controller[1]) == LOW) {
            delay(200); 
            return selected;
        }
    }
}

void setup(){
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    mpu.begin();

    // 센서 측정 범위 세팅 (필요에 따라 조정)
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // 가속도 ±8g
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);     // 자이로 ±500 deg/s
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);   // 노이즈 필터링
    
    for (int i = 0; i < 500; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        angleOffset += g.gyro.z;
        delay(2);
    }
    angleOffset *= 0.002;
    time[1] = micros();
    
    int io[] = {0, 1};
    pinMode(controller[1], 2);
    for( int i=0; i<2; i++){
        pinMode(sensor[i],io[i]);
        for( int j=0; j<2; j++){ for (int k=1; k<=2; k++){ pinMode(motor[i][j][k], 1);}}
    }
}

void loop(){
    int s = select();
    for( int i=0; i<10; i++){ drive(route[s][i]); }
}