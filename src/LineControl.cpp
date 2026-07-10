#include "LineControl.h"

#include <cmath>

#include "RobotConfig.h"

float wheel_distance_to_rotations(float distance_m)
{
    return distance_m / (2.0f * RobotConfig::PI * RobotConfig::WHEEL_RADIUS_M);
}

float rack_distance_to_rotations(float distance_m)
{
    return distance_m / (2.0f * RobotConfig::PI * RobotConfig::PINION_PITCH_M);
}

void follow_line(float line_angle_rad, float previous_line_angle_rad,
                 DCMotor& left_motor, DCMotor& right_motor,
                 float wheel_velocity_max_rad_s)
{
    const float linear_velocity_m_s = RobotConfig::SPEED_FACTOR *
                                      wheel_velocity_max_rad_s *
                                      RobotConfig::WHEEL_RADIUS_M;
    const float angular_velocity_rad_s = RobotConfig::LINE_KP * line_angle_rad +
                                         RobotConfig::LINE_KP_NL * line_angle_rad * std::fabs(line_angle_rad) +
                                         RobotConfig::LINE_KD * (line_angle_rad - previous_line_angle_rad);

    const float left_wheel_rad_s = linear_velocity_m_s / RobotConfig::WHEEL_RADIUS_M +
                                   RobotConfig::WHEEL_BASE_M * angular_velocity_rad_s /
                                   (2.0f * RobotConfig::WHEEL_RADIUS_M);
    const float right_wheel_rad_s = linear_velocity_m_s / RobotConfig::WHEEL_RADIUS_M -
                                    RobotConfig::WHEEL_BASE_M * angular_velocity_rad_s /
                                    (2.0f * RobotConfig::WHEEL_RADIUS_M);

    left_motor.setVelocity(left_wheel_rad_s / (2.0f * RobotConfig::PI));
    right_motor.setVelocity(right_wheel_rad_s / (2.0f * RobotConfig::PI));
}
