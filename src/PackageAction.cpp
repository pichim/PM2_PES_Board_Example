#include "PackageAction.h"

#include <cmath>

#include "LineControl.h"
#include "RobotConfig.h"

void PackageAction::start()
{
    phase = Phase::MOVE_BACK;
}

bool PackageAction::update(DCMotor& left_motor, DCMotor& right_motor, DCMotor& rack_motor)
{
    switch (phase) {
        case Phase::MOVE_BACK:
            left_start_rotation = left_motor.getRotation();
            right_start_rotation = right_motor.getRotation();
            left_motor.setRotation(left_start_rotation - wheel_distance_to_rotations(RobotConfig::ACTION_MOVE_BACK_M));
            right_motor.setRotation(right_start_rotation - wheel_distance_to_rotations(RobotConfig::ACTION_MOVE_BACK_M));
            phase = Phase::WAIT_MOVE_BACK;
            break;

        case Phase::WAIT_MOVE_BACK:
            if (std::fabs(left_motor.getRotation() -
                          (left_start_rotation - wheel_distance_to_rotations(RobotConfig::ACTION_MOVE_BACK_M))) <
                    RobotConfig::ACTION_POSITION_TOLERANCE_ROT &&
                std::fabs(right_motor.getRotation() -
                          (right_start_rotation - wheel_distance_to_rotations(RobotConfig::ACTION_MOVE_BACK_M))) <
                    RobotConfig::ACTION_POSITION_TOLERANCE_ROT) {
                phase = Phase::LOWER_RACK;
            }
            break;

        case Phase::LOWER_RACK:
            rack_start_rotation = rack_motor.getRotation();
            rack_motor.setRotation(rack_start_rotation - rack_distance_to_rotations(RobotConfig::ACTION_LOWER_RACK_M));
            phase = Phase::WAIT_LOWER_RACK;
            break;

        case Phase::WAIT_LOWER_RACK:
            if (std::fabs(rack_motor.getRotation() -
                          (rack_start_rotation - rack_distance_to_rotations(RobotConfig::ACTION_LOWER_RACK_M))) <
                RobotConfig::ACTION_POSITION_TOLERANCE_ROT) {
                phase = Phase::RESTORE_RACK;
            }
            break;

        case Phase::RESTORE_RACK:
            rack_motor.setRotation(rack_distance_to_rotations(RobotConfig::INITIAL_RACK_POSITION_M));
            phase = Phase::WAIT_RESTORE_RACK;
            break;

        case Phase::WAIT_RESTORE_RACK:
            if (std::fabs(rack_motor.getRotation() -
                          rack_distance_to_rotations(RobotConfig::INITIAL_RACK_POSITION_M)) <
                RobotConfig::RACK_HOME_TOLERANCE_ROT) {
                phase = Phase::IDLE;
                return true;
            }
            break;

        case Phase::IDLE:
            break;
    }

    return false;
}

void PackageAction::reset()
{
    phase = Phase::IDLE;
}

bool PackageAction::isBusy() const
{
    return phase != Phase::IDLE;
}
