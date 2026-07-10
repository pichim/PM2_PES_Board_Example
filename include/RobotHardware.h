#ifndef ROBOT_HARDWARE_H
#define ROBOT_HARDWARE_H

#include "mbed.h"
#include "DCMotor.h"
#include "SensorBar.h"
#include "ColorSensor.h"
#include "IMU.h"
#include "Servo.h"

struct RobotHardware {
    DCMotor& left_motor;
    DCMotor& right_motor;
    DCMotor& rack_motor;
    SensorBar& sensor_bar;
    ColorSensor& colour_sensor;
    IMU& imu;
    Servo& roll_servo;
    Servo& pitch_servo;
    DigitalOut& motor_enable;
};

#endif
