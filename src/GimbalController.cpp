#include "GimbalController.h"

#include "RobotConfig.h"

namespace {
constexpr float SERVO_PULSE_MIN = 0.035f;
constexpr float SERVO_PULSE_MAX = 0.130f;
constexpr float ANGLE_MIN_RAD = -RobotConfig::PI / 2.0f;
constexpr float ANGLE_MAX_RAD = RobotConfig::PI / 2.0f;
constexpr float NORMALISED_ANGLE_GAIN = 1.0f / RobotConfig::PI;
constexpr float NORMALISED_ANGLE_OFFSET = 0.5f;
}

GimbalController::GimbalController(Servo& roll_servo, Servo& pitch_servo)
    : roll_servo(roll_servo), pitch_servo(pitch_servo)
{
    this->roll_servo.calibratePulseMinMax(SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    this->pitch_servo.calibratePulseMinMax(SERVO_PULSE_MIN, SERVO_PULSE_MAX);
    reset();
}

void GimbalController::update(float roll_rad, float pitch_rad)
{
    if (!roll_servo.isEnabled()) {
        roll_servo.enable();
    }
    if (!pitch_servo.isEnabled()) {
        pitch_servo.enable();
    }

    if (ANGLE_MIN_RAD <= roll_rad && roll_rad <= ANGLE_MAX_RAD) {
        roll_servo.setPulseWidth(NORMALISED_ANGLE_GAIN * roll_rad + NORMALISED_ANGLE_OFFSET);
    }
    if (ANGLE_MIN_RAD <= pitch_rad && pitch_rad <= ANGLE_MAX_RAD) {
        pitch_servo.setPulseWidth(NORMALISED_ANGLE_GAIN * pitch_rad + NORMALISED_ANGLE_OFFSET);
    }
}

void GimbalController::reset()
{
    roll_servo.setPulseWidth(0.5f);
    pitch_servo.setPulseWidth(0.5f);
}
