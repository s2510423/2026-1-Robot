#include "Cart.h"

Motor leftFront(6,7,2);
Motor leftBack(8,9,3);
Motor rightFront(10,11,4);
Motor rightBack(12,13,5);

Motors left = {leftFront, leftBack};
Motors right = {rightFront, rightBack};

Mobile mobile(left, right);

SensorServo sensorServo(44);
Sensor sensor(22, 23);
Radar radar(sensor, sensorServo);

Display display;
Joystick joystick(A1, A0, 14);
Interface interface(display, joystick);

Husky husky;
Gyro gyro;

Cart cart(mobile, radar, gyro, husky, interface);

void setup(){}
void loop(){ cart.drive(); }
