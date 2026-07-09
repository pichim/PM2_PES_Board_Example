#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

#include "DCMotor.h"
#include <Eigen/Dense>

#include "SensorBar.h"

#define M_PIf 3.14159265358979323846f // pi

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

    // sensor bar
    const float bar_dist = 0.114f; // distance from wheel axis to leds on sensor bar / array in meters
    SensorBar sensor_bar(PB_9, PB_8, bar_dist);

    float angle = 0.0f;


    DigitalOut enable_motors(PB_ENABLE_DCMOTORS);

    const float voltage_max = 12.0f; // maximum voltage of battery packs, adjust this to
                                   // 6.0f V if you only use one battery pack
    const float gear_ratio = 100.0f;
    const float kn = 140.0f / voltage_max; // [rad/s/V] speed constant of the motor

    DCMotor motor_M2(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio, kn, voltage_max);
    DCMotor motor_M1(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, gear_ratio, kn, voltage_max);

    // differential drive kimatics
    const float r_wheel = 0.0665f / 2.0f;
    const float b_wheel = 0.162f;

    // robot tranforms

    Eigen::Matrix2f Cwheel2robot;
    Cwheel2robot << r_wheel / 2.0f, r_wheel/ 2.0f,
                     r_wheel / b_wheel, -r_wheel / b_wheel;
    Eigen::Vector2f robot_coord = {0.0f, 0.0f};
    Eigen::Vector2f wheel_speed = {0.0f, 0.0f};


    // rotational velocity controller
    const float Kp{5.0f};
    const float wheel_vel_max = 2.0f * M_PIf * motor_M2.getMaxPhysicalVelocity();

    // enable motors
    enable_motors = 1;

    // set desired velcities 
    // robot_coord(0) = 0.15f; // [m/s] forward velocity
    // robot_coord(1) = 0.1f; // [rad/s] angular

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
    // DigitalOut led1(PB_9);

    // --- adding variables and objects and applying functions starts here ---

    // start timer
    main_task_timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        // --- code that runs every cycle at the start goes here ---

        if (do_execute_main_task) {

            // --- code that runs when the blue button was pressed goes here ---

            // only update sensor bar angle if a led is triggered
            if (sensor_bar.isAnyLedActive())
            {
                angle = sensor_bar.getAvgAngleRad();
            }

            // control algorithm for robot velocities
            Eigen::Vector2f robot_coord = {0.5f * wheel_vel_max * r_wheel,  // half of the max. forward velocity
                                        Kp * angle                    }; // simple proportional angle controller

            wheel_speed = Cwheel2robot.inverse() * robot_coord;
           // setpoints for the dc motors in rps
            if ((wheel_speed(0) / (2.0f * M_PIf) > motor_M1.getMaxVelocity()) || (wheel_speed(1) / (2.0f * M_PIf) > motor_M2.getMaxVelocity()))
                printf("Warning: Desired wheel speed exceeds max. wheel speed. motion may be inacurate\n");
            motor_M1.setVelocity(wheel_speed(0) / (2.0f * M_PIf)); // set a desired speed for speed controlled dc motors M1
            motor_M2.setVelocity(wheel_speed(1) / (2.0f * M_PIf)); // set a desired speed for speed controlled dc motors M2
        } else {
            // the following code block gets executed only once
            if (do_reset_all_once) {
                do_reset_all_once = false;

                // --- variables and objects that should be reset go here ---

                // reset variables and objects
                motor_M1.setVelocity(0.0f);
                motor_M2.setVelocity(0.0f);
            }
        }

        // toggling the user led
        user_led = !user_led;

        printf("wheel speed M1: %f rps, wheel speed M2: %f rps\n", wheel_speed(0) / (2.0f * M_PIf), wheel_speed(1) / (2.0f * M_PIf));
        printf("max wheel speed M1: %f rps, max wheel speed M2: %f rps\n", motor_M1.getMaxVelocity(), motor_M2.getMaxVelocity());
        printf("sensor bar angle: %f rad, %f deg\n", angle, angle * 180.0f / M_PIf);

        // --- code that runs every cycle at the end goes here ---

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
