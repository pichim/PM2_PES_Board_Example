#ifndef GIMBAL_CONTROLLER_H
#define GIMBAL_CONTROLLER_H

#include "Servo.h"

class GimbalController {
public:
    GimbalController(Servo& roll_servo, Servo& pitch_servo);

    void update(float roll_rad, float pitch_rad);
    void reset();

private:
    Servo& roll_servo;
    Servo& pitch_servo;
};

#endif
