#ifndef LINE_CONTROL_H
#define LINE_CONTROL_H

#include "DCMotor.h"

float wheel_distance_to_rotations(float distance_m);
float rack_distance_to_rotations(float distance_m);

void follow_line(float line_angle_rad, float previous_line_angle_rad,
                 DCMotor& left_motor, DCMotor& right_motor,
                 float wheel_velocity_max_rad_s);

#endif
