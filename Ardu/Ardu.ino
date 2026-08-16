

const unsigned long route[4][10][2];
unsigned long time[4];
const int motor[2][2][3] = {
    { {2, 6, 7},{4, 10, 11} },
    { {3, 8, 9},{5, 12, 13} }
};
const int sensor[2] = {22, 23}, controller[] = {A1, 14};
const int thres = 30;


void set(int dir, int pin[2][3]) {
    int hl[3][2] = { {1,1}, {0,1}, {1,0} }; // 정지, 전진, 후진  
    for (int i=0; i<=1; i++) { for (int j=0; j<=1; j++) { digitalWrite(pin[j][i+1], io[dir][i])} };
}
void direction(int dir) {
    // 0:직진, 1:좌, 2:우, 3:정지 4:후진
    int dircode[5][2] = { {1,1},{2,1},{1,2},{0,0},{2,2} };
    for (int i=0; i<=1; i++) { set(dircode[dir][i], motor[i]); }
}
void motoron(int intensity){ for (int i=0; i<=1; i++) { analogWrite(motor[i][j][0], intensity); } }

bool near() {
    digitalWrite(sensor[1],0);
    delayMicroseconds(2);
    digitalWrite(sensor[1],1);
    delayMicroseconds(10);
    digitalWrite(sensor[1],0);
    unsigned long duration = pulseIn(sensor[0],1,20000)
    float dist = duration * 0.0343 / 2.0;
    return (dist<thres);
}

void drive( int direction, unsigned long duration) {
    direction(direction);
    motoron(255);
    for(int i=0; i<=3; i++){ time[i]=millis(); }
    for(; time[0]-time[1]<=duration; time[0]=millis()){ 
        if( near() ){
            direction(3);
            time[2]=millis();
        }
        else{
            direction(direction);
            time[1]+=time[2]-time[3];
            time[2]=time[3]=millis();
        }
    }
}
int select(){
    // ui 이용
    // 0 ~ 3
    //8반 ~ 11반
}


void setup(){
    int io[2] = {1,0,1,2};
    for( int i=0; i<2; i++){
        pinMode(sensor[i],io[i]);
        pinMode(ui[i],io[i+2]);
        for( int j=0; j<2; j++){ for (int k=1; k<=2; k++){ pinMode(motor[i][j][k], 1)}}
    }
}

void loop(){
    int s = select();
    for( i=0; i<10; i++){ drive(route[s][i][0], route[s][i][1]);}
}