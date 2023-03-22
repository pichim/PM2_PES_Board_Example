/*
* Project: pm2_pes_board_example
* Date: 22.03.2023
* Authors: Raphael Romann, Yves Guldimann
* Modul: PM2
* Version: 0.0.0.0
*/
#include <cstdio>
#include <mbed.h>
#include <ctime>

#include "PM2_Drivers.h"

# define M_PI 3.14159265358979323846 // number pi, an example in case you need it


bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(PC_13);  // create InterruptIn interface object to evaluate user button falling and rising edge (no blocking code in ISR)
void user_button_pressed_fcn(); // custom functions which get executed when user button gets pressed, definition below

// main runs as an own thread
int main()
{
    // attach button fall function to user button object, button has a pull-up resistor
    user_button.fall(&user_button_pressed_fcn);
    // while loop gets executed every main_task_period_ms milliseconds (simple aproach to repeatedly execute main)
    const int main_task_period_ms = 50; // define main task period time in ms e.g. 50 ms -> main task runs 20 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task every main_task_period_ms
    // led on nucleo board
    DigitalOut user_led(LED1);       // create DigitalOut object to command user led
    // additional led
    DigitalOut additional_led(PB_9); // create DigitalOut object to command extra led (you need to add an aditional resistor, e.g. 220...500 Ohm)

    // mechanical button
    DigitalIn mechanical_button(PC_5); // create DigitalIn object to evaluate extra mechanical button, you need to specify the mode for proper usage, see below
    mechanical_button.mode(PullUp);    // set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential

    // ----- Variablen ----
    
    // --- States and actual state for the machine
    // others

    const int GRYPER_STATE_INIT = 0; 
    const int GRYPER_STATE_ARM_DOWN_1 = 1;
    const int GRYPER_STATE_FORWARD_1 = 2;
    const int GRYPER_STATE_SET_ARM = 3;
    const int GRYPER_STATE_ROTATE = 4;
    const int GRYPER_STATE_BALANCE = 5;
    const int GRYPER_STATE_DETACH = 6;
    const int GRYPER_STATE_ARM_DOWN_2 = 7;
    const int GRYPER_STATE_FORWARD_2 = 8;
    const int GRYPER_STATE_FINAL = 9;
    const int GRYPER_STATE_RESET = 10;
    int gryper_state_actual = GRYPER_STATE_INIT;

    // ----- Motoren ----

    
    
    // ----- Sensoren ----




     // this loop will run forever
    while (true) {

        main_task_timer.reset();

        if (do_execute_main_task) {

            if (mechanical_button.read()) {


            }

            // visual feedback that the main task is executed, setting this once would actually be enough
            additional_led = 1;

            //state_machine
            switch(gryper_state_actual){

                case GRYPER_STATE_INIT:

                    if(mechanical_button == 1)

                    gryper_state_actual = GRYPER_STATE_ARM_DOWN_1;

                    else if(button2) {
                        gryper_state_actual = GRYPER_STATE_RESET;
                    } else {
                        gryper_state_actual = GRYPER_STATE_INIT;

                    }
                    break;

                case GRYPER_STATE_ARM_DOWN_1:
                
                    gryper_state_actual = GRYPER_STATE_ROTATE;
                    break;

                case GRYPER_STATE_ROTATE:
                
                    gryper_state_actual = GRYPER_STATE_BALANCE;
                    break;

                case GRYPER_STATE_BALANCE:
                
                    gryper_state_actual = GRYPER_STATE_DETACH;
                    break;

                case GRYPER_STATE_DETACH:
                
                    gryper_state_actual = GRYPER_STATE_ARM_DOWN_2;
                    break;

                case GRYPER_STATE_ARM_DOWN_2:
                
                    gryper_state_actual = GRYPER_STATE_FORWARD_2;
                    break;

                case GRYPER_STATE_FORWARD_2:
                
                    gryper_state_actual = GRYPER_STATE_FINAL;
                    break;

                case GRYPER_STATE_FINAL:
                
                    gryper_state_actual = GRYPER_STATE_INIT;
                    break;

                case GRYPER_STATE_RESET:
                
                    gryper_state_actual = GRYPER_STATE_INIT;
                    break;
            }

        } else {

            if (do_reset_all_once) {
                do_reset_all_once = false;

                additional_led = 0;
            }            
        }

        // toggling the user led
        user_led = !user_led;

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

void user_button_pressed_fcn()
{
    // do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    if (do_execute_main_task) do_reset_all_once = true;
}