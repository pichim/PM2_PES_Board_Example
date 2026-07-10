#include "MissionController.h"

#include "LineControl.h"
#include "RobotConfig.h"

void MissionController::update(RobotHardware& hardware, const SensorReadings& readings)
{
    previous_line_angle_rad = line_angle_rad;
    if (readings.line_visible) {
        line_angle_rad = readings.line_angle_rad;
    }

    switch (state) {
        case RobotState::INITIAL:
            hardware.motor_enable = 1;
            hardware.rack_motor.setRotation(rack_distance_to_rotations(RobotConfig::INITIAL_RACK_POSITION_M));
            state = RobotState::FIND_LINE;
            break;

        case RobotState::FIND_LINE:
            state = RobotState::PICKUP_APPROACH;
            break;

        case RobotState::PICKUP_APPROACH:
            followCurrentLine(hardware);
            if (isSupportedColour(readings.colour) &&
                pickup_count < RobotConfig::NUMBER_OF_PACKAGES &&
                pickup_markers.confirmNewColour(readings.colour)) {
                pickup_colour_in_progress = readings.colour;
                pickup_action.start();
                state = RobotState::PICKUP_ALIGNMENT;
            }
            break;

        case RobotState::PICKUP_ALIGNMENT:
            followCurrentLine(hardware);
            if (hardware.sensor_bar.getMeanFourAvgBitsCenter() >= 0.75f &&
                hardware.sensor_bar.getMeanFourAvgBitsOuter() >= 0.25f &&
                pickup_count < RobotConfig::NUMBER_OF_PACKAGES) {
                hardware.left_motor.setVelocity(0.0f);
                hardware.right_motor.setVelocity(0.0f);
                state = RobotState::PICKUP_ACTION;
            }
            break;

        case RobotState::PICKUP_ACTION:
            if (pickup_action.update(hardware.left_motor, hardware.right_motor, hardware.rack_motor)) {
                pickup_markers.markCompleted(pickup_colour_in_progress);
                pickup_colour_in_progress = 0;
                pickup_count++;
                state = RobotState::PICKUP_RESUME;
            }
            break;

        case RobotState::PICKUP_RESUME:
            state = pickup_count < RobotConfig::NUMBER_OF_PACKAGES
                        ? RobotState::PICKUP_APPROACH
                        : RobotState::DELIVERY_APPROACH;
            break;

        case RobotState::DELIVERY_APPROACH:
            followCurrentLine(hardware);
            if (isSupportedColour(readings.colour) &&
                delivery_count < RobotConfig::NUMBER_OF_PACKAGES &&
                delivery_markers.confirmNewColour(readings.colour)) {
                delivery_colour_in_progress = readings.colour;
                delivery_action.start();
                state = RobotState::DELIVERY_ALIGNMENT;
            }
            break;

        case RobotState::DELIVERY_ALIGNMENT:
            followCurrentLine(hardware);
            if (hardware.sensor_bar.getMeanFourAvgBitsCenter() >= 0.75f &&
                hardware.sensor_bar.getMeanFourAvgBitsOuter() >= 0.25f &&
                delivery_count < RobotConfig::NUMBER_OF_PACKAGES) {
                hardware.left_motor.setVelocity(0.0f);
                hardware.right_motor.setVelocity(0.0f);
                state = RobotState::DELIVERY_ACTION;
            }
            break;

        case RobotState::DELIVERY_ACTION:
            if (delivery_action.update(hardware.left_motor, hardware.right_motor, hardware.rack_motor)) {
                delivery_markers.markCompleted(delivery_colour_in_progress);
                delivery_colour_in_progress = 0;
                delivery_count++;
                state = RobotState::DELIVERY_RESUME;
            }
            break;

        case RobotState::DELIVERY_RESUME:
            state = delivery_count < RobotConfig::NUMBER_OF_PACKAGES
                        ? RobotState::DELIVERY_APPROACH
                        : RobotState::APPROACH_END;
            break;

        case RobotState::APPROACH_END:
            followCurrentLine(hardware);
            if (hardware.sensor_bar.getMeanThreeAvgBitsLeft() >= 0.66f &&
                hardware.sensor_bar.getMeanThreeAvgBitsRight() <= 0.33f) {
                state = RobotState::MISSION_END;
            }
            break;

        case RobotState::MISSION_END:
            hardware.left_motor.setVelocity(0.0f);
            hardware.right_motor.setVelocity(0.0f);
            break;

        case RobotState::EMERGENCY_STOP:
            break;
    }
}

void MissionController::reset()
{
    state = RobotState::INITIAL;
    pickup_count = 0;
    delivery_count = 0;
    line_angle_rad = 0.0f;
    previous_line_angle_rad = 0.0f;
    pickup_colour_in_progress = 0;
    delivery_colour_in_progress = 0;
    pickup_markers.reset();
    delivery_markers.reset();
    pickup_action.reset();
    delivery_action.reset();
}

bool MissionController::isFinished() const
{
    return state == RobotState::MISSION_END;
}

void MissionController::followCurrentLine(RobotHardware& hardware) const
{
    const float wheel_velocity_max_rad_s = 2.0f * RobotConfig::PI *
                                           hardware.right_motor.getMaxPhysicalVelocity();
    follow_line(line_angle_rad, previous_line_angle_rad,
                hardware.left_motor, hardware.right_motor,
                wheel_velocity_max_rad_s);
}

bool MissionController::isSupportedColour(int colour) const
{
    return colour == RobotConfig::RED || colour == RobotConfig::YELLOW ||
           colour == RobotConfig::GREEN || colour == RobotConfig::BLUE;
}
