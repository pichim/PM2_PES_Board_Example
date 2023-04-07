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
#include <math.h>

#include "PM2_Drivers.h"

# define M_PI 3.14159265358979323846 // number pi, an example in case you need it

// ---- Vehicle Variables ----
# define WHEEL_DIAMETER     50.0    // in mm
# define ARM_LENGTH         160.0   // in mm
# define GRYPER_HIGHT        30.0    // in mm


// ---- Operation Variables ----
# define DISTANCE_1         200      // in mm
# define DISTANCE_2         200      // in mm    
# define ANGEL_SET_ARM      45       // in Grad


// ---- predefined Variables ----
# define HURDLE_HIGHT       100      // in mm

#include <main.h>
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
    float max_speed_rps_M1 = 0.5f;
    const int M1_gear = 195;
    const float maxAccelerationRPS_M1 = 1.0f;

    const float counts_per_turn_M1 = 20.0f * 78.125f;      // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn_M1 = 72.0f / 12.0f;                    // define motor constant in RPM/V
    const float k_gear_M1 = M1_gear / 78.125f;              // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    const float kp_M1 = 0.1f;

    FastPWM pwm_M1(PB_13);                              // Pin is correct!
    EncoderCounter  encoder_M1(PA_6, PC_7);             // Pin is correct!
    PositionController positionController_M1(counts_per_turn_M1 * k_gear_M1, kn_M1 / k_gear_M1, max_voltage, pwm_M1, encoder_M1);
    positionController_M1.setSpeedCntrlGain(kp_M1 * k_gear_M1);   // adjust internal speed controller gain, this is just an example
    positionController_M1.setMaxVelocityRPS(max_speed_rps_M1); // adjust max velocity for internal speed controller
    // fuer ruck beseitigung; maximale beschleunigung festssetzen
    positionController_M1.setMaxAccelerationRPS(maxAccelerationRPS_M1);


    // ----- M2 (closed-loop position controlled) -----
    float max_speed_rps_M2 = 0.5f;
    const int M2_gear = 488;
    const float maxAccelerationRPS_M2 = 1.0f;

    const float counts_per_turn_M2 = 20.0f * 78.125f;      // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn_M2 = 50.0f / 12.0f;                    // define motor constant in RPM/V
    const float k_gear_M2 = M2_gear / 78.125f;              // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    const float kp_M2 = 0.1f;

    FastPWM pwm_M2(PA_9);                       // Pin is correct!
    EncoderCounter  encoder_M2(PB_6, PB_7);     // Pin is correct!
    PositionController positionController_M2(counts_per_turn_M2 * k_gear_M2, kn_M2 / k_gear_M2, max_voltage, pwm_M2, encoder_M2);
    positionController_M2.setSpeedCntrlGain(kp_M2 * k_gear_M2);   // adjust internal speed controller gain, this is just an example
    positionController_M2.setMaxVelocityRPS(max_speed_rps_M2); // adjust max velocity for internal speed controller
    // fuer ruck beseitigung; maximale beschleunigung festssetzen
    positionController_M2.setMaxAccelerationRPS(maxAccelerationRPS_M2);

    // ----- M3 (closed-loop position controlled) -----
   /* const int M3_gear = 0;
    FastPWM pwm_M3(PA_10);                      // Pin is correct!
    EncoderCounter  encoder_M3(PA_0, PA_1);     // Pin is correct!
    */


    // ---------- Sensoren ----------
    // ---------- States and actual state for the machine ----------

    const int GRYPER_STATE_INIT = 0; 
    const int GRYPER_STATE_ARM_DOWN_1 = 1;
    const int GRYPER_STATE_FORWARD_1 = 2;
    const int GRYPER_STATE_SET_ARM = 3;
    const int GRYPER_STATE_ROTATE = 4;
    //const int GRYPER_STATE_BALANCE = 5;
    //const int GRYPER_STATE_DETACH = 6;
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

         /*   if (mechanical_button.read()) {
                //printf("\nSTART M1");
                enable_motors = 1;
                if (positionController_M1.getRotation() <= 0.1f){
                    positionController_M1.setDesiredRotation(convertDistanceToRadians(200)); 

                } 
            }
            printf(" ----- TEST -----:Rotation: %f", positionController_M1.getRotation());
            if (positionController_M1.getRotation() >= convertDistanceToRadians(201)) { 
                printf("\nRESET M1");
                positionController_M1.setDesiredRotation(0.0f);
            }
        */    
    
            // visual feedback that the main task is executed, setting this once would actually be enough
            additional_led = 1;

            //state_machine
            switch(gryper_state_actual){

                case GRYPER_STATE_INIT:

                    if(mechanical_button == 1){
                        // Start the loop

                        gryper_state_actual = GRYPER_STATE_ARM_DOWN_1;


                        gryper_state_actual = GRYPER_STATE_SET_ARM;

                        // For testing set the state that you want to test
                        enable_motors = 1;


                        gryper_state_actual = GRYPER_STATE_FORWARD_1;

                        enable_motors = 1;
                        // For testing set the state that you want to test
            
            

                    } else if(button2) {
                        // for the resetloop
                        gryper_state_actual = GRYPER_STATE_RESET;
                    } else {

                        // set state to init state
                        gryper_state_actual = GRYPER_STATE_INIT;

                        //gryper_state_actual = GRYPER_STATE_INIT;

                    }
                    break;


                case GRYPER_STATE_ARM_DOWN_1:

                    if (positionController_M2.getRotation() <= 0.1f){
                        positionController_M2.setDesiredRotation(0.125f); // 1.0f = 360° 
                        
                    }

                    //gryper_state_actual = GRYPER_STATE_FORWARD_1;
                    break;
                
                case GRYPER_STATE_FORWARD_1:

                    if (positionController_M1.getRotation() <= 0.1){
                        positionController_M1.setDesiredRotation(convertDistanceToRotation(157.08, WHEEL_DIAMETER)); 
                        printf("1RAD: %f", convertDistanceToRotation(157.08, WHEEL_DIAMETER));
                    } 
            
                /*    if (positionController_M1.getRotation() >= convertDistanceToRadians(201)) { 
                        printf("\nRESET M1");
                        positionController_M1.setDesiredRotation(0.0f);
                    }
                */
                    //gryper_state_actual = GRYPER_STATE_SET_ARM;
                    break;
                
                case GRYPER_STATE_SET_ARM:
                    /*  lift the arm and set it to the hurdle 
                        1. drive arm in 0 position
                        2. calculate angle
                        3. drive angel
                        4. drive a little bit forward
                    */

                    // 1. drive arm in 0 position
                    positionController_M2.setDesiredRotation(0.0f);

                    // 2. calculate angle
                    double angle_B = calcAngleSetArm(); // in rad
                    float bogenlaenge = get_way_from_rad(angle_B); // in mm
                    float rotation = convertDistanceToRotation(bogenlaenge, ARM_LENGTH);
                    printf("ANGLE: %f [m]\tWEG: %f\tROT: %f",angle_B, bogenlaenge, rotation);

                    // 3. drive angle
                    //positionController_M2.setDesiredRotation(dist);
                                   
                    //gryper_state_actual = GRYPER_STATE_ROTATE;
                    break;

                /*
                case GRYPER_STATE_ROTATE:
                
                    gryper_state_actual = GRYPER_STATE_BALANCE;
                    break;

                case GRYPER_STATE_BALANCE:
                
                    gryper_state_actual = GRYPER_STATE_DETACH;
                    break;
                */

                case GRYPER_STATE_DETACH:
                
                    gryper_state_actual = GRYPER_STATE_ARM_DOWN_2;
                    break;

                case GRYPER_STATE_ARM_DOWN_2:

                    if (positionController_M2.getRotation() <= 0.1f){
                        positionController_M2.setDesiredRotation(0.125f); // 1.0f = 360° 
                        
                    }
                
                    //gryper_state_actual = GRYPER_STATE_FORWARD_2;
                    break;

                case GRYPER_STATE_FORWARD_2:

                    if (positionController_M1.getRotation() <= 0.1f){
                        positionController_M1.setDesiredRotation(convertDistanceToRotation(157.08, WHEEL_DIAMETER)); 
                        printf("2RAD: %f", convertDistanceToRotation(157.08, WHEEL_DIAMETER));
                    }

                    //gryper_state_actual = GRYPER_STATE_FINAL;
                    break;

                case GRYPER_STATE_FINAL:

                    if (positionController_M2.getRotation() <= 0.1f){ // Move Gryper back vertical to the car
                        positionController_M2.setDesiredRotation(-0.125f); // 1.0f = 360° 
                        
                    }

                    //gryper_state_actual = GRYPER_STATE_INIT;
                    break;

                case GRYPER_STATE_RESET:
                
                    gryper_state_actual = GRYPER_STATE_INIT;
                    break;
            }

            printf("\nEncoder M1: %3d\tPosition M1: %3.3f\tEncoder M2: %3d\tPosition M2: %3.3f", encoder_M1.read(), positionController_M1.getRotation(), encoder_M2.read(), positionController_M2.getRotation());

        } else {

            if (do_reset_all_once) {
                printf("\nALL DONE");
                do_reset_all_once = false;

                additional_led = 0;
                positionController_M1.setDesiredRotation(0.0f);
                positionController_M2.setDesiredRotation(0.0f);
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

// General Functions
float convertDistanceToRadians(float distanceInMillimeters, float diameter) {
    float u = diameter * M_PI;
    return (u / 2) * M_PI / distanceInMillimeters;

}

float get_way_from_rad(float angle){
     return angle * ARM_LENGTH;
}


float convertDistanceToRotation(float distanceInMillimeters, float diameter) {
    return distanceInMillimeters / (diameter * M_PI);
}

// Functions for STEP 3
double calcAngleSetArm(void) {
    return asin((HURDLE_HIGHT + GRYPER_HIGHT)/ARM_LENGTH);;
}

