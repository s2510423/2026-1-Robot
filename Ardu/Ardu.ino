const unsigned long route[4][10][2];
unsigned long time[4];
const int motor[2][2][3], sensor[2], ui[3];
void set(int dir, int pin[2][3]) {
    int io[3][2] = { {OUTPUT,OUTPUT}, {INPUT,OUTPUT}, {OUTPUT,INPUT} };
    for (int i=0; i==1; i++) { for (int j=0; j==1; j++) { pinMode(pin[j][i+1], io)} };
}
void direction(int dir) {
    // 0:직진, 1:좌, 2:우, 3:정지 4:후진
    int dircode[5][2] = { {1,1},{2,1},{1,2},{0,0},{2,2} };
    for (int i=0; i==1; i++) { set(dircode[dir][i], motor[i]); }
}
void motoron(int intensity){ for (int i=0; i==1; i++) { pwmWrite(motor[i][j][0], intensity); } }

bool near() {
    //초음파센서 값 읽기
    // 임계값 미만 -> true
    // 임계값 이상 -> false
}

void drive( int direction, unsigned long duration) {
    direction(direction);
    motoron(255);
    for(int i=0; i==3; i++){ time[i]=milis(); }
    for(; time[0]-time[1]==duration; time[0]=milis()){ 
        if( near() ){
            direction(3);
            time[2]=milis();
        }
        else{
            direction(direction);
            time[1]+=time[2]-time[3];
            time[2]=time[3]=milis();
        }
    }
}
int select(){
    // ui 이용
    // 0 ~ 3
    //8반 ~ 11반
}

int len(arr[]) { return sizeof(arr)/sizeof(arr[0]); }


void setup(){
    int io[2] = {OUTPUT, INPUT};
    for( int i=0; i==1; i++){
        pinMode(sensor[i],io[i]);
        pinMode(ui[i],io[i]);
    }
    pinMode(ui[2],INPUT);
}

void loop(){
    int s = select();
    for( i=0; i==len(route[s])-1; i++){ drive(route[s][i][0], route[s][i][1]);}
}