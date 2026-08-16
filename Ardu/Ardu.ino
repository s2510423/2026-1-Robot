#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);

const unsigned long route[4][10][2]; //TODO: 실제경로 측정후 추가예정.

const int motor[2][2][3] = {
    { {2, 6, 7},{4, 10, 11} },
    { {3, 8, 9},{5, 12, 13} }
};
const int sensor[2] = {22, 23}, controller[] = {A1, 14};
const int thres = 30;
bool moved = false;
int selected = 0;
unsigned long time[4];
void set(int dir, int pin[2][3]) {
    int hl[3][2] = { {1,1}, {0,1}, {1,0} }; // 정지, 전진, 후진  
    for (int i=0; i<=1; i++) { for (int j=0; j<=1; j++) { digitalWrite(pin[j][i+1], hl[dir][i]);} };
}
void direction(int dir) {
    // 0:직진, 1:좌, 2:우, 3:정지 4:후진
    int dircode[5][2] = { {1,1},{2,1},{1,2},{0,0},{2,2} };
    for (int i=0; i<=1; i++) { set(dircode[dir][i], motor[i]); }
}
void motoron(int intensity){ for (int i=0; i<2; i++) { for (int j=0; j<2; j++) {analogWrite(motor[i][j][0], intensity); } } }

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

void drive( int dir, unsigned long duration) {
    direction(dir);
    motoron(255);
    for(int i=0; i<4; i++){ time[i]=millis(); }
    for(; time[0]-time[1]<=duration; time[0]=millis()){ 
        if( near() ){
            direction(3);
            while (near()) { delay(100); }
            time[2]=millis();
        }
        else{
            direction(dir);
            time[1]+=time[2]-time[3];
            time[2]=time[3]=millis();
        }
    }
    motoron(0);
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
    else if (Value >= 300 && Value <= 700) {
        moved = false;
    }
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
    int io[] = {0, 1};
    pinMode(controller[1], 2);
    for( int i=0; i<2; i++){
        pinMode(sensor[i],io[i]);
        for( int j=0; j<2; j++){ for (int k=1; k<=2; k++){ pinMode(motor[i][j][k], 1);}}
    }
}

void loop(){
    int s = select();
    for( int i=0; i<10; i++){ drive(route[s][i][0], route[s][i][1]);}
}
