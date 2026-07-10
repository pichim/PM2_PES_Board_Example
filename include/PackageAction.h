#ifndef PACKAGE_ACTION_H
#define PACKAGE_ACTION_H

#include "DCMotor.h"

class PackageAction {
public:
    void start();
    bool update(DCMotor& left_motor, DCMotor& right_motor, DCMotor& rack_motor);
    void reset();
    bool isBusy() const;

private:
    enum class Phase {
        IDLE,
        MOVE_BACK,
        WAIT_MOVE_BACK,
        LOWER_RACK,
        WAIT_LOWER_RACK,
        RESTORE_RACK,
        WAIT_RESTORE_RACK
    };

    Phase phase = Phase::IDLE;
    float left_start_rotation = 0.0f;
    float right_start_rotation = 0.0f;
    float rack_start_rotation = 0.0f;
};

#endif
