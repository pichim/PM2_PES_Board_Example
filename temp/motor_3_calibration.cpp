#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

// motors inludes
#include "FastPWM.h"
#include "DCMotor.h"

bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(BUTTON1);   // create DebounceIn to evaluate the user button
void toggle_do_execute_main_fcn(); // custom function which is getting executed when user
                                   // button gets pressed, definition at the end

// main runs as an own thread
int main()
{
    // motor1 (open loop) PWM output
    // FastPWM pwm_M1(PB_PWM_M1);

    DigitalOut enable_motors(PB_ENABLE_DCMOTORS);

    // m2 for velocity closed loop control
    const float voltage_max = 12.0f;

    // const float gear_ratio_M2 = 100.0f; 
    // const float speed_constant = 140.0f;
    // const float kn_M2 = speed_constant / voltage_max; // [rad/s/V] speed constant of the motor

    // DCMotor motor_M2(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio_M2, kn_M2, voltage_max);


    
    // enable the motion planner for smooth movements
    // motor_M2.enableMotionPlanner();
    // limit max. velocity to half physical possible velocity
    // motor_M2.setMaxVelocity(motor_M2.getMaxPhysicalVelocity() * 0.5f);
    // limit max. acceleration to half of the default acceleration
    // motor_M2.setMaxAcceleration(motor_M2.getMaxAcceleration() * 0.5f);

    const float speed_constant_100 = 140.0f;
    const float gear_ratio_100 = 100.0f;
    const float kn_100 = speed_constant_100 / voltage_max; // [

    DCMotor motor_M1(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio_100, kn_100, voltage_max);
    DCMotor motor_M2(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, gear_ratio_100, kn_100, voltage_max);

    motor_M1.enableMotionPlanner();
    motor_M1.setMaxVelocity(motor_M1.getMaxPhysicalVelocity() * 0.5f);

    motor_M2.enableMotionPlanner();
    motor_M2.setMaxVelocity(motor_M2.getMaxPhysicalVelocity() * 0.5f);

    const float speed_constant = 36.0f;
    const float gear_ratio_M3 = 390.63f;
    const float kn_M3 = speed_constant / voltage_max; // [rad/s/V] speed constant of the motor

    DCMotor motor_M3(PB_PWM_M3, PB_ENC_A_M3, PB_ENC_B_M3, gear_ratio_M3, kn_M3, voltage_max);

    motor_M3.enableMotionPlanner();
    motor_M3.setMaxVelocity(motor_M3.getMaxPhysicalVelocity() * 0.5f);


    // attach button fall function address to user button object
    user_button.fall(&toggle_do_execute_main_fcn);

    // while loop gets executed every main_task_period_ms milliseconds, this is a
    // simple approach to repeatedly execute main
    const int main_task_period_ms = 20; // define main task period time in ms e.g. 20 ms, therefore
                                        // the main task will run 50 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task
                                        // every main_task_period_ms

    // led on nucleo board
    DigitalOut user_led(LED1);

    // additional led
    // create DigitalOut object to command extra led, you need to add an additional resistor, e.g. 220...500 Ohm
    // a led has an anode (+) and a cathode (-), the cathode needs to be connected to ground via the resistor
    DigitalOut led1(PB_9);

    // --- adding variables and objects and applying functions starts here ---

    // start timer
    main_task_timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        // --- code that runs every cycle at the start goes here ---

        if (do_execute_main_task) {

            // --- code that runs when the blue button was pressed goes here ---

            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;
            // pwm_M1.write(0.75f);
            // motor_M2.setVelocity(motor_M2.getMaxVelocity() * 0.5f);
            enable_motors = 1;
            motor_M3.setRotation(0.5f);
            motor_M1.setVelocity(motor_M1.getMaxVelocity() * 0.0f);
            motor_M2.setVelocity(motor_M2.getMaxVelocity() * 0.0f);
        } else {
            // the following code block gets executed only once
            if (do_reset_all_once) {
                do_reset_all_once = false;

                // --- variables and objects that should be reset go here ---

                // reset variables and objects
                led1 = 0;
                // pwm_M1.write(0.5f);
                motor_M1.setVelocity(0.0f);
                motor_M2.setVelocity(0.0f);
                motor_M3.setRotation(0.0f);
                // enable_motors = 0; 
                // motor_M3.setRotation(0.0f);
            }
        }

        // toggling the user led
        user_led = !user_led;

        // --- code that runs every cycle at the end goes here ---
        // printf("motor velocity: %f \n", motor_M2.getVelocity());
        printf("motor rotation: %f \n", motor_M3.getRotation());
        printf("motor velocity M1: %f \n", motor_M1.getVelocity());
        printf("motor velocity M2: %f \n", motor_M2.getVelocity());
        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = duration_cast<milliseconds>(main_task_timer.elapsed_time()).count();
        if (main_task_period_ms - main_task_elapsed_time_ms < 0)
            printf("Warning: Main task took longer than main_task_period_ms\n");
        else
            thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

void toggle_do_execute_main_fcn()
{
    // toggle do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    // set do_reset_all_once to true if do_execute_main_task changed from false to true
    if (do_execute_main_task)
        do_reset_all_once = true;
}