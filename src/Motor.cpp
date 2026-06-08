#include "Motor.h"
#include <Arduino.h>

Motor::Motor(char p_pwm, char p_fore, char p_back)
: pin{p_pwm, p_fore, p_back} {

}
