#ifndef ROBOT_TYPES_H
#define ROBOT_TYPES_H

enum class RobotState {
    INITIAL,
    FIND_LINE,
    PICKUP_APPROACH,
    PICKUP_ALIGNMENT,
    PICKUP_ACTION,
    PICKUP_RESUME,
    DELIVERY_APPROACH,
    DELIVERY_ALIGNMENT,
    DELIVERY_ACTION,
    DELIVERY_RESUME,
    EMERGENCY_STOP,
    APPROACH_END,
    MISSION_END
};

struct SensorReadings {
    int colour = 0;
    bool line_visible = false;
    float line_angle_rad = 0.0f;
    float roll_rad = 0.0f;
    float pitch_rad = 0.0f;
};

#endif
