#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

namespace RobotConfig {
constexpr float PI = 3.14159265358979323846f;
constexpr int MISSION_PERIOD_MS = 50;

constexpr float WHEEL_DIAMETER_M = 0.0665f;
constexpr float WHEEL_RADIUS_M = WHEEL_DIAMETER_M / 2.0f;
constexpr float WHEEL_BASE_M = 0.153f;
constexpr float SENSOR_BAR_DISTANCE_M = 0.037f;

constexpr float VOLTAGE_MAX = 12.0f;
constexpr float GEAR_RATIO = 100.0f;
constexpr float SPEED_CONSTANT = 140.0f;
constexpr float RACK_GEAR_RATIO = 390.63f;
constexpr float RACK_SPEED_CONSTANT = 36.0f;
constexpr float RACK_SPEED_PER_VOLT = RACK_SPEED_CONSTANT / VOLTAGE_MAX;
constexpr float SPEED_PER_VOLT = SPEED_CONSTANT / VOLTAGE_MAX;
constexpr float PINION_PITCH_M = 8e-3f;

constexpr float SPEED_FACTOR = 0.1f;
constexpr float LINE_KP = 2.0f;
constexpr float LINE_KD = 0.0f;
constexpr float LINE_KP_NL = 0.0f;

constexpr int RED = 3;
constexpr int YELLOW = 4;
constexpr int GREEN = 5;
constexpr int BLUE = 7;
constexpr int NUMBER_OF_PACKAGES = 4;
constexpr int COLOUR_CONFIRMATION_CYCLES = 3;

constexpr float INITIAL_RACK_POSITION_M = 20e-3f;
constexpr float ACTION_MOVE_BACK_M = 50e-3f;
constexpr float ACTION_LOWER_RACK_M = 10e-3f;
constexpr float ACTION_POSITION_TOLERANCE_ROT = 0.01f;
constexpr float RACK_HOME_TOLERANCE_ROT = 0.001f;
}

#endif
