#ifndef COLOUR_MARKER_FILTER_H
#define COLOUR_MARKER_FILTER_H

#include "RobotConfig.h"

class ColourMarkerFilter {
public:
    bool confirmNewColour(int colour);
    void markCompleted(int colour);
    bool isCompleted(int colour) const;
    void reset();

private:
    int completed_colours[RobotConfig::NUMBER_OF_PACKAGES] = {0, 0, 0, 0};
    int completed_count = 0;
    int candidate_colour = 0;
    int candidate_cycles = 0;
};

#endif
