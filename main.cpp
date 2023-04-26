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

// ------------- Vehicle Variables -------------
# define WHEEL_DIAMETER     30.0    // in mm
# define ARM_LENGTH         160.0   // in mm
# define GRYPER_HIGHT        30.0    // in mm


// ------------- Operation Variables -------------
# define DISTANCE_1         50      // in mm (gemessen auf der Bahn 440mm)
# define DISTANCE_2         50      // in mm    
# define ANGEL_SET_ARM      45       // in Grad

bool arm_0_position = 0;
bool arm_down = 0;
bool arm_down_2 = 0;
bool forward_1 = 0;
bool forward_2 = 0;
bool rotate_full = 0;
bool adjust_ok = 0;
bool detach_ok = 0;
bool detach_forward_ok;

// for GRYPER_STATE_SET_ARM
double angle_B = 0.0, angle_rot = 0.0;
float bogenlaenge = 0.0, rotation = 0.0, act_pos = 0.0, act_pos_m1;

// for GRYPER_STATE_DETACH
double angle_detach = 0;
double angle_adjust = 0;


// ------------- predefined Variables -------------
# define HURDLE_HIGHT       100      // in mm

#include <main.h>
// main runs as an own thread


int main()
{
    // attach button fall function to user button object, button has a pull-up resistor
    user_button.fall(&user_button_pressed_fcn); //Blue User Button on Nucleo Board
    // while loop gets executed every main_task_period_ms milliseconds (simple aproach to repeatedly execute main)
    const int main_task_period_ms = 50; // define main task period time in ms e.g. 50 ms -> main task runs 20 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task every main_task_period_ms
    // led on nucleo board
    

    // ------------- Variablen -------------

    // ---------- Buttons ----------
    DigitalIn mechanical_button(PC_5); 
    mechanical_button.mode(PullUp);

    DigitalIn btn_start(PC_9);   // create DigitalIn object to evaluate extra mechanical button, you need to specify the mode for proper usage, see below
    btn_start.mode(PullUp); // set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential
    
    DigitalIn btn_reset_vehicle(PC_8);
    btn_reset_vehicle.mode(PullUp); // set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential
    
    DigitalIn btn_reset_all(PC_6);
    btn_reset_all.mode(PullUp); // set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential
    
    DigitalIn btn_reserve(PB_12);
    btn_reserve.mode(PullUp); // set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential


    // ------------- Leds -------------
    DigitalOut user_led(LED1);       // create DigitalOut object to command user led
    // additional led
    DigitalOut additional_led(PB_9); // create DigitalOut object to command extra led (you need to add an aditional resistor, e.g. 220...500 Ohm)

    // ------------- Vehicle geometry -------------

    // ------------- Motoren -------------
    DigitalOut enable_motors(PB_15);                    // create DigitalOut object to enable dc motors
    const float max_voltage = 12.0f;                    // Voltage for DC-Motors


    // ------------- M1 (closed-loop position controlled) -------------
    float max_speed_rps_M1 = 0.2f;
    const int M1_gear = 100;
    const float maxAccelerationRPS_M1 = 1.0f;

    const float counts_per_turn_M1 = 20.0f * 78.125f;      // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn_M1 = 180.0f / 12.0f;                    // define motor constant in RPM/V
    const float k_gear_M1 = M1_gear / 78.125f;              // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    const float kp_M1 = 0.1f;

    FastPWM pwm_M1(PB_13);                              // Pin is correct!
    EncoderCounter  encoder_M1(PA_6, PC_7);             // Pin is correct!
    PositionController positionController_M1(counts_per_turn_M1 * k_gear_M1, kn_M1 / k_gear_M1, max_voltage, pwm_M1, encoder_M1);
    positionController_M1.setSpeedCntrlGain(kp_M1 * k_gear_M1);   // adjust internal speed controller gain, this is just an example
    positionController_M1.setMaxVelocityRPS(max_speed_rps_M1); // adjust max velocity for internal speed controller
    // fuer ruck beseitigung; maximale beschleunigung festssetzen
    positionController_M1.setMaxAccelerationRPS(maxAccelerationRPS_M1);


    // ------------- M2 (closed-loop position controlled) -------------
    float max_speed_rps_M2 = 0.03f;
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

    // ------------- M3 (closed-loop position controlled) -------------
   /* const int M3_gear = 0;
    FastPWM pwm_M3(PA_10);                      // Pin is correct!
    EncoderCounter  encoder_M3(PA_0, PA_1);     // Pin is correct!
    */


    // ------------- Sensoren -------------
    // ------------- States and actual state for the machine -------------

    const int GRYPER_STATE_INIT = 0; 
    const int GRYPER_STATE_ARM_DOWN_1 = 1;
    const int GRYPER_STATE_FORWARD_1 = 2;
    const int GRYPER_STATE_SET_ARM = 3;
    const int GRYPER_STATE_ROTATE = 4;
    const int GRYPER_STATE_DETACH = 5;
    const int GRYPER_STATE_ARM_DOWN_2 = 6;
    const int GRYPER_STATE_FORWARD_2 = 7;
    const int GRYPER_STATE_FINAL = 8;
    const int GRYPER_STATE_RESET = 9;
    const int GRYPER_TEST = 11;
    int gryper_state_actual = GRYPER_STATE_INIT;


     // this loop will run forever
    while (true) {

        main_task_timer.reset();

        //printf("Mechanical Button State %d\n", mechanical_button.read());
        
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
                        enable_motors = 1;

                        gryper_state_actual = GRYPER_STATE_ARM_DOWN_1;

                        
                        // For testing set the state that you want to test
                        //gryper_state_actual = GRYPER_STATE_ARM_DOWN_1;
            
            

                    } else if(btn_reset_vehicle) {
                        // for the resetloop
                        gryper_state_actual = GRYPER_STATE_RESET;
                    } else {

                        // set state to init state
                        gryper_state_actual = GRYPER_STATE_INIT;

                        //gryper_state_actual = GRYPER_STATE_INIT;

                    }
                    break;


                case GRYPER_STATE_ARM_DOWN_1:
                    printf("Run STATE_ARM_DOWN_1\n");
                    /* Test1: Winkel ein bisschen kleiner machen
                    //  geschwindikeit i.o, 
                        übergang zu STATE 2 i.o */

                    if (!arm_down){
                        positionController_M2.setDesiredRotation(-0.222f); // 1.0f = 360°, 0.222f= 80° 
                        
                    }
                     if(positionController_M2.getRotation() <= -0.221f){  
                        arm_down = 1;  
                        gryper_state_actual = GRYPER_STATE_FORWARD_1;
                        printf("Set STATE_FORWARD_1\n");
                    }
                    break;
                
                case GRYPER_STATE_FORWARD_1:
                    printf("Run STATE_FORWARD_1\n");

                    /* Test1: 
                        Distanz fahren io,
                    */

                    positionController_M1.setDesiredRotation(convertDistanceToRotation(DISTANCE_1, WHEEL_DIAMETER)); // 1 Radumdrehung 94.25
                    printf("\n1RAD: %f\n", convertDistanceToRotation(DISTANCE_1, WHEEL_DIAMETER));

                    if(positionController_M1.getRotation() >= convertDistanceToRotation(DISTANCE_1, WHEEL_DIAMETER)-0.1f){
                        forward_1 = 1;
                        gryper_state_actual = GRYPER_STATE_SET_ARM;
                        printf("Set STATE_SET_ARM\n");
                        //gryper_state_actual = GRYPER_STATE_INIT;
                    }
                    break;
                
                case GRYPER_STATE_SET_ARM:
                    printf("Run STATE_SET_ARM\n");
                    /*  lift the arm and set it to the hurdle 
                        1. drive arm in 0 position
                        2. calculate angle
                        3. drive angel
                        4. drive a little bit forward
                    */

                    // 1. drive arm in 0 position
                    printf("START: GRYPER_STATE_SET_ARM\n");
                    if (!arm_0_position) {
                        positionController_M2.setDesiredRotation(0.0f);

                    }

                    if(positionController_M2.getRotation()>= -0.01f && positionController_M2.getRotation() <= 0.01f) {
                        printf("Reset 0 Position\n");
                        arm_0_position = 1;
                        arm_down_2 = 0; // RR: wür ich erst am schluss mache
                    }               

                    // 2. calculate angle
                    angle_B = calcAngleSetArm(); // in rad
                    bogenlaenge = get_way_from_rad(angle_B); // in mm
                    rotation = convertDistanceToRotation(bogenlaenge, ARM_LENGTH);
                    printf("ANGLE: %f [m]\tWEG: %f\tROT: %f\n",angle_B, bogenlaenge, rotation);

                    
                    
                    if (arm_0_position) {
                        printf("SET_ARM: drive angle\n");
                        positionController_M2.setDesiredRotation(rotation);
                    }

                    // 3. drive angle and drive backwords 0.25

                    if (positionController_M2.getRotation() >= 0.2) {       // RR: 0.2 muss noch angepasst werden
                        printf("SET_ARM: drive forward\n");
                        positionController_M1.setDesiredRotation(-0.25);
                    }
                    
                    // Set further STEP  
                    if(positionController_M2.getRotation() >= rotation-0.01f){
                        printf("SET_ARM: set next step\n");
                        gryper_state_actual = GRYPER_STATE_ROTATE;
                    }                               
                    break;
                
                case GRYPER_STATE_ROTATE:
                    printf("Run STATE_ROTATE\n");
                
                    /* drive angle over the hurdle
                        1. calc angle/dist: (360 - 2*angle_B)
                        2. drive angle 
                        3. drive adjust angle
                    */

                    // 1. calculate angle
                    angle_rot = 2 * M_PI - 2 * calcAngleSetArm();
                    bogenlaenge = get_way_from_rad(angle_rot); // in mm
                    rotation = convertDistanceToRotation(bogenlaenge, ARM_LENGTH);
                     rotation = 0.5; // RR: Nur zu test zwecken
                    printf("ANGLE ROT: %f \tWEG: %f\tROT: %f\n",angle_rot, bogenlaenge, rotation);
                    
                    // 2. drive angle
                    if (!rotate_full) {
                        positionController_M2.setDesiredRotation(rotation); // RR: + aktuelle Position
                        printf("ROTATE: gryper is rotating\n");
                    }
                    
                    // check if arm reaches the end position
                    printf("STATE ROTATE: Angle %f\n", positionController_M2.getRotation());
                    if (positionController_M2.getRotation() >= rotation - 0.001f) {
                        rotate_full = 1;
                        printf("ROTATE: full rotate OK\n");
                    }

                    // 3. adjust angle
                    angle_adjust = 0.01;
                    if (rotate_full && !adjust_ok) {
                        act_pos = positionController_M2.getRotation();
                        positionController_M2.setDesiredRotation(act_pos + angle_adjust);
                        adjust_ok = 1;
                        printf("ROTATE: gryper is adjusting\n");
                    }
                    // Set further STEP 
                    if (positionController_M2.getRotation() >= act_pos + angle_adjust && adjust_ok && rotate_full) {
                        gryper_state_actual = GRYPER_STATE_DETACH;
                        //gryper_state_actual = GRYPER_STATE_INIT;
                    }
                    break;

                case GRYPER_STATE_DETACH:
                    printf("Run STATE_DETACH\n");
                    /*  lift the arm and detach it from the hurdle 
                        1. lift arm and drive backword
                        2. drive 5cm forward
                    */
                    angle_detach = 0.125;

                    // 1. lift arm and drive backword
                    if (!detach_ok) {
                        printf("DETACH: is detaching\n");
                        act_pos = positionController_M2.getRotation();
                        positionController_M2.setDesiredRotation(act_pos + angle_detach);
                        positionController_M1.setDesiredRotation(convertDistanceToRotation(-20, WHEEL_DIAMETER) + positionController_M1.getRotation());
                        detach_ok = 1;
                    }
                    
                    // 2. drive 5cm forward
                    printf("DETACH: pos. M1 %f\n", positionController_M1.getRotation());
                    if (positionController_M2.getRotation() >= act_pos + angle_detach - 0.01 && detach_ok && !detach_forward_ok) {
                        printf("DETACH: drive forward\n");
                        positionController_M1.setDesiredRotation(convertDistanceToRotation(50, WHEEL_DIAMETER) + positionController_M1.getRotation()); 
                        act_pos_m1 = positionController_M1.getRotation();
                        detach_forward_ok = 1;
                        // RR: evt. ganz nach hinten fahren bis an das hinderniss und nur von dort aus eine bestimmte distanz fahren
                    }
                    
                    // Set further STEP 
                    
                    if (detach_forward_ok && detach_ok && positionController_M1.getRotation() >= act_pos_m1 + convertDistanceToRotation(50, WHEEL_DIAMETER) -0.01f) {
                        printf("DETACH: Finish\n");
                        //gryper_state_actual = GRYPER_STATE_ARM_DOWN_2;
                        gryper_state_actual = GRYPER_STATE_INIT;
                    }
                    
                    break;

                case GRYPER_STATE_ARM_DOWN_2:
                    printf("Run STATE_ARM_DOWN_2\n");

                    if (!arm_down_2){
                        positionController_M2.setDesiredRotation(1.222f); // 1.0f = 360°, 0.222f = 80°
                        
                    }
                    if(positionController_M2.getRotation() <= -0.221f){
                        arm_down_2 = 1;
                        //gryper_state_actual = GRYPER_STATE_FORWARD_2;
                        gryper_state_actual = GRYPER_STATE_INIT;
                    }
                    break;

                case GRYPER_STATE_FORWARD_2:
                    printf("Run STATE_FORWARD_2\n");

                    positionController_M1.setDesiredRotation(convertDistanceToRotation(94.25, WHEEL_DIAMETER)); 
                    printf("\n2RAD: %f\n", convertDistanceToRotation(94.25, WHEEL_DIAMETER));

                    if(positionController_M1.getRotation() <= convertDistanceToRotation(94.24, WHEEL_DIAMETER)-0.1f){
                        forward_2 = 1;
                        //gryper_state_actual = GRYPER_STATE_FINAL;
                        gryper_state_actual = GRYPER_STATE_INIT;
                    }
                    
                    break;

                case GRYPER_STATE_FINAL:
                    printf("Run STATE_FINAL\n");

                    if (positionController_M2.getRotation() <= 0.1f){ // Move Gryper back vertical to the car
                        positionController_M2.setDesiredRotation(1.0f); // 1.0f = 360° 
                        
                    }
                    if (positionController_M2.getRotation() >= 0.98f && positionController_M2.getRotation() <= 1.02f){
                        gryper_state_actual = GRYPER_STATE_INIT;
                    }   
                    break;

                case GRYPER_STATE_RESET:
                     printf("Run STATE_RESET\n");
                    gryper_state_actual = GRYPER_STATE_INIT;
                    break;
            }

            //printf("\nEncoder M1: %3d\tPosition M1: %3.3f\tEncoder M2: %3d\tPosition M2: %3.3f", encoder_M1.read(), positionController_M1.getRotation(), encoder_M2.read(), positionController_M2.getRotation());

        } else {

            if (do_reset_all_once) {
                printf("\nALL DONE\n");
                do_reset_all_once = false;

                additional_led = 0;
               // positionController_M1.setDesiredRotation(0.0f);
                //positionController_M2.setDesiredRotation(0.0f);
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

