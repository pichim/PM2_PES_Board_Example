#ifndef MISSION_CONTROLLER_H
#define MISSION_CONTROLLER_H

#include "RobotTypes.h"
#include "RobotHardware.h"
#include "ColourMarkerFilter.h"
#include "PackageAction.h"

class MissionController {
public:
    void update(RobotHardware& hardware, const SensorReadings& readings);
    void reset();
    bool isFinished() const;

private:
    RobotState state = RobotState::INITIAL;
    int pickup_count = 0;
    int delivery_count = 0;
    float line_angle_rad = 0.0f;
    float previous_line_angle_rad = 0.0f;
    int pickup_colour_in_progress = 0;
    int delivery_colour_in_progress = 0;

    ColourMarkerFilter pickup_markers;
    ColourMarkerFilter delivery_markers;
    PackageAction pickup_action;
    PackageAction delivery_action;

    void followCurrentLine(RobotHardware& hardware) const;
    bool isSupportedColour(int colour) const;
};

#endif
