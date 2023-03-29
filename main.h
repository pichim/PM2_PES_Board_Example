#pragma once 

#include <cstdio>
#include <mbed.h>
#include <ctime>

#include "PM2_Drivers.h"

#include "function.h"

# define M_PI 3.14159265358979323846 // number pi, an example in case you need it

// ---- Vehicle Variables ----
# define WHEEL_DIAMETER 50.0 


bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(PC_13);  // create InterruptIn interface object to evaluate user button falling and rising edge (no blocking code in ISR)
