#pragma once 


void user_button_pressed_fcn(); // custom functions which get executed when user button gets pressed, definition below
float convertDistanceToRotation(float distanceInMillimeters, float diameter); //custom function which calculate Radians from Distance
float convertDistanceRadians(float distanceInMillimeters, float diameter);

float get_way_from_rad(float angle);
double calcAngleSetArm(void);