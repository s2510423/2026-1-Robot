#include "Gyro.h"
#include <Adafruit_Sensor.h>

Gyro::Gyro()
: angle(0), angleOffset(0) {}
void Gyro::begin() {
    Wire.begin();
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
float Gyro::measure() {
    mpu.getEvent(&a, &g, &temp);
    float deltaAngle =  g.gyro.z;
    unsigned long currentTime = micros();
    float dt = (currentTime - time ) / 1000000.0;
    time = currentTime;
    angle += (deltaAngle - angleOffset) * (180.0 / PI) * dt;
    return angle;
}
void Gyro::setAngle(float newAngle){ angle = newAngle; }
float Gyro::getAngle(){ return angle; }
void Gyro::end(){
    mpu.enableSleep(true);
    Wire.end();
}
