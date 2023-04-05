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

// ---- Vehicle Variables ----
# define WHEEL_DIAMETER 50.0     // in mm
# define ARM_LENGTH = 160.0      // in mm


// ---- Operation Variables ----
# define DISTANCE_1 200         // in mm
# define DISTANCE_2 200         // in mm    
# define ANGEL_SET_ARM 45       // in Grad


// ---- predefined Varables ----
# define HURDLE_HIGHT 100       // in mm



bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(PC_13);  // create InterruptIn interface object to evaluate user button falling and rising edge (no blocking code in ISR)
void user_button_pressed_fcn(); // custom functions which get executed when user button gets pressed, definition below
float convertDistanceToRadians(float distanceInMillimeters); //custom function which calculate Radians from Distance

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



    // ---------- Variablen ----------

    // ---------- Buttons ----------

    DigitalIn mechanical_button(PC_5); 
    mechanical_button.mode(PullUp);
    int button1 = 0;
    int button2 = 0;
    int button3 = 0;

    // ---------- Vehicle geometry ----------

    // ---------- Motoren ----------
    DigitalOut enable_motors(PB_15);                    // create DigitalOut object to enable dc motors
    const float max_voltage = 12.0f;                    // Voltage for DC-Motors


    // ----- M1 (closed-loop position controlled) -----
    float max_speed_rps = 0.5f;
    const int M1_gear = 100;
    const float maxAccelerationRPS = 1.0f;

    const float counts_per_turn = 20.0f * 78.125f;      // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn = 180.0f / 12.0f;                    // define motor constant in RPM/V
    const float k_gear = 100.0f / 78.125f;              // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    const float kp = 0.1f;

    FastPWM pwm_M1(PB_13);                              // Pin is correct!
    EncoderCounter  encoder_M1(PA_6, PC_7);             // Pin is correct!
    PositionController positionController_M1(counts_per_turn * k_gear, kn / k_gear, max_voltage, pwm_M1, encoder_M1);
    positionController_M1.setSpeedCntrlGain(kp * k_gear);   // adjust internal speed controller gain, this is just an example
    positionController_M1.setMaxVelocityRPS(max_speed_rps); // adjust max velocity for internal speed controller
    // fuer ruck beseitigung; maximale beschleunigung festssetzen
    positionController_M1.setMaxAccelerationRPS(maxAccelerationRPS);


    // ----- M2 (closed-loop position controlled) -----
    const int M2_gear = 0;
    FastPWM pwm_M2(PA_9);                       // Pin is correct!
    EncoderCounter  encoder_M2(PB_6, PB_7);     // Pin is correct!
    

    // ----- M3 (closed-loop position controlled) -----
    const int M3_gear = 0;
    FastPWM pwm_M3(PA_10);                      // Pin is correct!
    EncoderCounter  encoder_M3(PA_0, PA_1);     // Pin is correct!



    // ---------- Sensoren ----------
    // ---------- States and actual state for the machine ----------

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
    const int GRYPER_TEST = 11;
    int gryper_state_actual = GRYPER_STATE_INIT;


     // this loop will run forever
    while (true) {

        main_task_timer.reset();

        if (do_execute_main_task) {

            if (mechanical_button.read()) {
                //printf("\nSTART M1");
                enable_motors = 1;
                positionController_M1.setDesiredRotation(1.5f); 


            }
            if (positionController_M1.getRotation() >= 1.45f) { 
                printf("\nRESET M1");
                positionController_M1.setDesiredRotation(0.0f);
            }
    
            // visual feedback that the main task is executed, setting this once would actually be enough
            additional_led = 1;

            //state_machine
            switch(gryper_state_actual){

                case GRYPER_STATE_INIT:

                    if(mechanical_button == 1){
                        // Start the loop
                        gryper_state_actual = GRYPER_STATE_ARM_DOWN_1;
                    } else if(button2) {
                        // for the resetloop
                        gryper_state_actual = GRYPER_STATE_RESET;
                    } else {
                        // set state to init state
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

            printf("\nEncoder M1: %3d\tPosition M1: %3.3f", encoder_M1.read(), positionController_M1.getRotation());

        } else {

            if (do_reset_all_once) {
                printf("\nALL DONE");
                do_reset_all_once = false;

                additional_led = 0;
                positionController_M1.setDesiredRotation(0.0f);
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

float convertDistanceToRadians(float distanceInMillimeters) {
    float u = WHEEL_DIAMETER * M_PI;
    return (u / 2) * M_PI / distanceInMillimeters;

}


