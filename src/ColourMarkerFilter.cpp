#include "ColourMarkerFilter.h"

bool ColourMarkerFilter::confirmNewColour(int colour)
{
    if (isCompleted(colour)) {
        candidate_colour = 0;
        candidate_cycles = 0;
        return false;
    }

    if (colour == candidate_colour) {
        candidate_cycles++;
    } else {
        candidate_colour = colour;
        candidate_cycles = 1;
    }

    if (candidate_cycles >= RobotConfig::COLOUR_CONFIRMATION_CYCLES) {
        candidate_colour = 0;
        candidate_cycles = 0;
        return true;
    }

    return false;
}

void ColourMarkerFilter::markCompleted(int colour)
{
    if (!isCompleted(colour) && completed_count < RobotConfig::NUMBER_OF_PACKAGES) {
        completed_colours[completed_count] = colour;
        completed_count++;
    }
}

bool ColourMarkerFilter::isCompleted(int colour) const
{
    for (int i = 0; i < completed_count; i++) {
        if (completed_colours[i] == colour) {
            return true;
        }
    }
    return false;
}

void ColourMarkerFilter::reset()
{
    completed_count = 0;
    candidate_colour = 0;
    candidate_cycles = 0;

    for (int& colour : completed_colours) {
        colour = 0;
    }
}
