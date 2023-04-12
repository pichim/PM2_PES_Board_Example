#include <iterator>
#include <mbed.h>

#include "PM2_Drivers.h"

# define M_PI 3.14159265358979323846 // number pi, an example in case you need it

//new
bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(PC_13);  // create InterruptIn interface object to evaluate user button falling and rising edge (no blocking code in ISR)
void user_button_pressed_fcn(); // custom functions which get executed when user button gets pressed, definition below
float total_distance_M2 = 0.0f;
float total_distance_M3 = 0.0f;
float calculate_distance_traveled(float rotations, float wheel_diameter)
{
     float wheel_radius_cm = wheel_diameter / 2.0f;
    float wheel_circumference_cm = 2.0f * M_PI * wheel_radius_cm;
    float distance = rotations * wheel_diameter * M_PI;
    float total_distance_M2 =+ distance * 10.0f;
    float total_distance_M3 =+ distance * 10.0f;
     printf("Distance 1M3: %f\n", total_distance_M2);
     printf("Distance 1M2: %f\n", total_distance_M3);
     float* ptr_total_distance_M3 = &total_distance_M3;
     
    return  total_distance_M3;
}

// main runs as an own thread
int main()
{
    user_button.fall(&user_button_pressed_fcn);
    // states and actual state for state machine
    const int ROBOT_STATE_INIT                = 0;
    const int ROBOT_STATE_FORWARD             = 1;
    const int ROBOT_STATE_BACKWARD            = 2;
    const int ROBOT_STATE_SLEEP               = 3;
    const int ROBOT_STATE_ARM_SETS_Angle      = 4;
    const int ROBOT_STATE_MOVE_ARM_AND_WHEELS = 5;
    const int ROBOT_STATE_SET_POSITION        = 6;
    const int ROBOT_STATE_REACH_GOAL          = 7;
    

    int robot_state_actual = ROBOT_STATE_INIT;
    // attach button fall function to user button object, button has a pull-up resistor
    

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


    // 78:1, 100:1, ... Metal Gearmotor 20Dx44L mm 12V CB
    DigitalOut enable_motors(PB_15); // create DigitalOut object to enable dc motors

    FastPWM pwm_M1(PB_13); // motor M1 is used open-loop

    FastPWM pwm_M2(PA_9);  // motor M2 is closed-loop speed controlled (angle velocity)
    FastPWM pwm_M3(PA_10); // motor M3 is closed-loop position controlled (angle controlled)

    EncoderCounter  encoder_M1(PA_6, PC_7); // create encoder objects to read in the encoder counter values, since M1 is used open-loop no encoder would be needed for operation, this is just an example
    EncoderCounter  encoder_M2(PB_6, PB_7);
    EncoderCounter  encoder_M3(PA_0, PA_1);
   
   
    // create SpeedController and PositionController objects, default parametrization is for 78.125:1 gear box
    const float max_voltage = 12.0f;               // define maximum voltage of battery packs, adjust this to 6.0f V if you only use one batterypack
    const float counts_per_turn = 20.0f * 78.125f; // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn = 180.0f / 12.0f;               // define motor constant in RPM/V
    const float k_gear = 100.0f / 78.125f;         // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    const float kp = 0.1f;                         // define custom kp, this is the default speed controller gain for gear box 78.125:1
          float distance = 0.0;
          float turn_for_1cm = 0.106f;  // entspricht 1 cm
          long double turn_for_30cm = 3.19f;
    const float wheel_diameter = 0.3f;   
          float total_distance_M3 = 0.0f;
          float total_distance_M2 = 0.0f; 
         
          
    PositionController positionController_M1(counts_per_turn, kn, max_voltage, pwm_M2, encoder_M1);
    positionController_M1.setSpeedCntrlGain(kp * k_gear);
    float max_speed_rps1 = 0.5f;
    positionController_M1.setMaxVelocityRPS(max_speed_rps1);

    PositionController positionController_M2(counts_per_turn, kn, max_voltage, pwm_M2, encoder_M2); // default 78.125:1 gear box  with default contoller parameters
    positionController_M2.setSpeedCntrlGain(kp * k_gear);
    float max_speed_rps2 = 0.5f;
    positionController_M2.setMaxVelocityRPS(max_speed_rps2);
    // SpeedController speedController_M2(counts_per_turn * k_gear, kn / k_gear, max_voltage, pwm_M2, encoder_M2); // parameters adjusted to 100:1 gear

    // PositionController positionController_M3(counts_per_turn, kn, max_voltage, pwm_M3, encoder_M3); // default 78.125:1 gear with default contoller parameters

    
    PositionController positionController_M3(counts_per_turn * k_gear, kn / k_gear, max_voltage, pwm_M3, encoder_M3); // parameters adjusted to 100:1 gear, we need a different speed controller gain here
    positionController_M3.setSpeedCntrlGain(kp * k_gear);   // adjust internal speed controller gain, this is just an example
    float max_speed_rps3 = 0.5f; // define maximum speed that the position controller is changig the speed, has to be smaller or equal to kn * max_voltage
    positionController_M3.setMaxVelocityRPS(max_speed_rps3); // adjust max velocity for internal speed controller
   float rotations_M3 = 0.0f;
                        float  distance_M3 = calculate_distance_traveled(rotations_M3, wheel_diameter); 
                        total_distance_M3 += distance_M3;
                        // set a desired speed for speed controlled dc motors M
                        
                          


    main_task_timer.start();
    
    // this loop will run forever
    while (true) {

        main_task_timer.reset();

        if (do_execute_main_task) {

        
            // visual feedback that the main task is executed, setting this once would actually be enough
            additional_led = 1;

            // commanding the servos
           

            // state machine
            switch (robot_state_actual) {

                case ROBOT_STATE_INIT:

        
                  //pressiing mechanical buttom

                    enable_motors = 1; // enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled

                    robot_state_actual = ROBOT_STATE_FORWARD;
                    break;

                case ROBOT_STATE_FORWARD: // robot moves forward when the motors are enabled

                    if (mechanical_button.read()) {
                     
                        positionController_M2.setDesiredRotation(5.0f); // set a desired speed for speed controlled dc motors M
                       
                        positionController_M3.setDesiredRotation(5.0f); //bewegt sich bis zum hindernis
                        rotations_M3 = positionController_M3.getRotation();
                         total_distance_M3 = calculate_distance_traveled(rotations_M3, wheel_diameter); 
                        
                      printf("Distance 00: %f\n", total_distance_M3);
                     
        
    }
               
                    break;
                   

                case ROBOT_STATE_BACKWARD:  // not sure of needed in this code //evtl. brauchen wir es nicht

                    if ( positionController_M3.getRotation() >= 5.0f)  {
            
                        positionController_M3.setDesiredRotation(-3.0f);
                        positionController_M1.setDesiredRotation(0.0f);
                     rotations_M3 = positionController_M3.getRotation();
                    distance_M3 = calculate_distance_traveled(rotations_M3, wheel_diameter); 
                        positionController_M2.setDesiredRotation(3.0f);
                        robot_state_actual = ROBOT_STATE_SLEEP;
                    }
                    break;

                case ROBOT_STATE_SLEEP:  // robot sleeps when the desired position in rad is achieved

                    if (positionController_M3.getRotation()>= 500.0f) { //wenn M1 und M2 die gewünshte rotation bis zum hindernis erreicht hat
                        enable_motors = 0; // motor wird ausgeschaltet

                       positionController_M2.setDesiredRotation(0.0f);
                       positionController_M3.setDesiredRotation(0.0f);
                        rotations_M3 = positionController_M3.getRotation();
                    distance_M3 = calculate_distance_traveled(rotations_M3, wheel_diameter); 
                  
                       robot_state_actual = ROBOT_STATE_BACKWARD;
                        
                        // robot_state_actual is not changed, there for the state machine remains in here until the blue button is pressed again
                    }
                    break;
                
                case ROBOT_STATE_ARM_SETS_Angle: // arm sets angle when the desired Position in Rad from case ROBOT_SLEEP is achieved 
                if (total_distance_M3 >= 29.0 ) {
                  
                positionController_M3.setDesiredRotation(-3.0f); // set a desired rotation for position controlled dc motors M3
                  rotations_M3 = positionController_M3.getRotation();
                     distance_M3 = calculate_distance_traveled(rotations_M3, wheel_diameter); 
                    
                robot_state_actual = ROBOT_STATE_MOVE_ARM_AND_WHEELS;
                }
                    break;

                case ROBOT_STATE_MOVE_ARM_AND_WHEELS: //moves forward while moving arm
                if (positionController_M3.getRotation() <= 6.16f){
                    
                    positionController_M1.setDesiredRotation(5.5f);
                    positionController_M2.setDesiredRotation(5.5f);
                    positionController_M3.setDesiredRotation(5.5f);
                    robot_state_actual = ROBOT_STATE_SET_POSITION;
                }
                    break;

                case ROBOT_STATE_SET_POSITION: //sets its position after going above hurdle
                if (positionController_M3.getRotation() <= 0.05f){ // wenn der arm die endposition erreicht (umdrehung bis zum anschlag)
                    positionController_M1.setDesiredRotation(0.02f); //muss sich nach hinten bewegen bis zur Hürde
                    positionController_M2.setDesiredRotation(0.02f); // gleich wie M1
                    robot_state_actual = ROBOT_STATE_REACH_GOAL;     // die reichweite müsste man testen
                }
                    break;

                case ROBOT_STATE_REACH_GOAL:
                if (positionController_M1.getRotation() <= 0.05f ){ //wenn die gewünschte position im ROBOT_SET erreicht wurde
                    positionController_M1.setDesiredRotation(3.18f); // rotation bis zum endziel
                    positionController_M2.setDesiredRotation(3.18f); // set a desired speed for speed controlled dc motors M2
                    enable_motors = 0;
                    robot_state_actual = ROBOT_STATE_INIT;
                }

                    break;

                default:
                
                    // do nothing
                    break;
            }

        } else {

            if (do_reset_all_once) {
                do_reset_all_once = false;

                pwm_M1.write(0.5f);
                positionController_M1.setDesiredRotation(0.0f);
                positionController_M2.setDesiredRotation(0.0f);
                positionController_M3.setDesiredRotation(0.0f);
                robot_state_actual = ROBOT_STATE_INIT;
                


                additional_led = 0;
            }   
            

        }
        

        // toggling the user led
        user_led = !user_led;

        // do only output via serial what's really necessary, this makes your code slow
              

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