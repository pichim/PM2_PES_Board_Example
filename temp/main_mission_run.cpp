#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

// DC motors
#include "DCMotor.h"

// eigen library for matrix operations
#include <Eigen/Dense>

// sensor bar
#include "SensorBar.h"

#define M_PIf 3.14159265358979323846f // pi
#define MISSION_PERIOD 20 //
// robot params
#define WHEEL_DIAMETER 0.0665f // wheel diameter in meters
#define WHEEL_RADIUS (WHEEL_DIAMETER / 2.0f) // wheel radius in meters
#define WHEEL_BASE 0.153f // wheelbase, distance from wheel to wheel in meters
#define SENSOR_BAR_DISTANCE 0.037f // distance from wheel axis to leds on sensor bar / array in meters
#define VOLTAGE_MAX 12.0f // maximum voltage of battery packs, adjust this to 6.0f V if you only use one battery pack
#define GEAR_RATIO 100.0f
#define SPEED_CONSTANT 140.0f // speed constant of the motor
#define KN (SPEED_CONSTANT / VOLTAGE_MAX) // [rad/s/V] speed constant of the motor

// controller parameters
#define KP 5.0f // proportional gain for the rotational velocity controller
#define KD 0 // derivative gain for the rotational velocity controller
#define KP_NL 0.0f // proportional gain for the non-linear controller


// mission params
#define NUMBER_OF_PICKUPS 4
bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(BUTTON1);   // create DebounceIn to evaluate the user button
void toggle_do_execute_main_fcn(); // custom function which is getting executed when user
                                   // button gets pressed, definition at the end

void follow_line(float angle, float prev_angle, DCMotor &motor_M1, DCMotor &motor_M2, Eigen::Matrix2f &Crobot2wheel, Eigen::Vector2f &robot_velocities, Eigen::Vector2f &wheel_velocities, const float wheel_vel_max)
{
    robot_velocities = {0.4f * wheel_vel_max * WHEEL_RADIUS,
                        KP * angle + KP_NL * angle * fabs(angle) + KD * (angle - prev_angle)};

    wheel_velocities = Crobot2wheel * robot_velocities;

    motor_M1.setVelocity(wheel_velocities(0) / (2.0f * M_PIf));
    motor_M2.setVelocity(wheel_velocities(1) / (2.0f * M_PIf));
}
// main runs as an own thread
int main()
{

    
enum RobotState 
{
    INITIAL, 
    FIND_LINE,
    // FOLLOW_LINE,
    PICKUP_APPROACH,
    PICKUP_ACTION,
    PICKUP_RESUME,
    // STEP_APPROACH,
    // STEP_CLIMB,
    DELIVERY_APPROACH,
    DELIVERY_ACTION,
    DELIVERY_RESUME,
    EMERGENCY_STOP,
    APPROACH_END,
    MISSION_END
} robot_state = RobotState::INITIAL;

    // attach button fall function address to user button object
    user_button.fall(&toggle_do_execute_main_fcn);

    // while loop gets executed every main_task_period_ms milliseconds, this is a
    // simple approach to repeatedly execute main
    const int main_task_period_ms = MISSION_PERIOD; // define main task period time in ms e.g. 20 ms, therefore
                                        // the main task will run 50 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task
                                        // every main_task_period_ms

    // led on nucleo board
    DigitalOut user_led(LED1);

    // additional led
    // create DigitalOut object to command extra led, you need to add an additional resistor, e.g. 220...500 Ohm
    // a led has an anode (+) and a cathode (-), the cathode needs to be connected to ground via the resistor
    DigitalOut led1(PB_9);

    // Motor setup
    DCMotor motor_M1(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, GEAR_RATIO, KN, VOLTAGE_MAX);
    DCMotor motor_M2(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, GEAR_RATIO, KN, VOLTAGE_MAX);

    DigitalOut enable_motors(PB_ENABLE_DCMOTORS);

    enable_motors = 1;
    
    const float wheel_vel_max = 2.0f * M_PIf * motor_M2.getMaxPhysicalVelocity();



    // sensor bar setup
    SensorBar sensor_bar(PB_9, PB_8, SENSOR_BAR_DISTANCE, true);
    float angle = 0.0f;


    // robot transforms
    // velocity kinematics
    Eigen::Matrix2f Cwheel2robot;
    Cwheel2robot << WHEEL_RADIUS / 2.0f, WHEEL_RADIUS / 2.0f,
                    WHEEL_RADIUS / WHEEL_BASE, -WHEEL_RADIUS / WHEEL_BASE;
    Eigen::Matrix2f Crobot2wheel = Cwheel2robot.inverse();
    Eigen::Vector2f wheel_velocities{0.0f, 0.0f};
    Eigen::Vector2f robot_velocities{0.0f, 0.0f};

    // TODO: position kinematics

    // --- adding variables and objects and applying functions starts here ---

    // start timer
    main_task_timer.start();

    int pickup_counter = 0;
    int delivery_counter = 0;
    int stop_timer = 0;
    const float action_time = 2.0f;

    // this loop will run forever
    while (true) 
    {
        main_task_timer.reset();

        // --- code that runs every cycle at the start goes here ---

        if (do_execute_main_task) 
        {

            // --- code that runs when the blue button was pressed goes here ---

            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;

            float prev_angle;
            prev_angle = angle;
            if (sensor_bar.isAnyLedActive()) 
            {
                angle = sensor_bar.getAvgAngleRad();
            }
            switch (robot_state) 
            {
                case INITIAL:
                    // code for INITIAL state
                    // TODO: check if all components are working // enable motors
                    robot_state = RobotState::FIND_LINE;
                    enable_motors = 1;

                    break;
                case FIND_LINE:
                    // code for FIND_LINE state
                    // TODO: move forward a bit and turn a bit to the left
                    robot_state = RobotState::PICKUP_APPROACH;
                    break;
                case PICKUP_APPROACH:
                    // code for PICKUP_APPROACH state
                    // follow line
                    follow_line(angle, prev_angle, motor_M1, motor_M2, Crobot2wheel, robot_velocities, wheel_velocities, wheel_vel_max);

                    if ((sensor_bar.getMeanFourAvgBitsCenter() >= 0.75f)
                        && (sensor_bar.getMeanFourAvgBitsOuter() >= 0.25f)
                    && (pickup_counter < 4)) 
                    {
                        
                        robot_state = RobotState::PICKUP_ACTION;
                    }

                    break;
                case PICKUP_ACTION:
                    // code for PICKUP_ACTION state
                    stop_timer++;
                    motor_M1.setVelocity(0.0f);
                    motor_M2.setVelocity(0.0f);
                    if (stop_timer >= action_time * 1000 / main_task_period_ms) 
                    {
                        stop_timer = 0;
                        pickup_counter++;
                        robot_state = RobotState::PICKUP_RESUME;
                    }
                    break;
                case PICKUP_RESUME:
                    // code for PICKUP_RESUME state
                    if (pickup_counter < NUMBER_OF_PICKUPS)
                    {
                        robot_state = RobotState::PICKUP_APPROACH;
                    } 
                    if (pickup_counter == NUMBER_OF_PICKUPS) 
                    {
                        robot_state = RobotState::DELIVERY_APPROACH;
                    }
                    
                    break;
                case DELIVERY_APPROACH:
                    // code for DELIVERY_APPROACH state
                    // for now stop
                    follow_line(angle, prev_angle, motor_M1, motor_M2, Crobot2wheel, robot_velocities, wheel_velocities, wheel_vel_max);
                    if ((sensor_bar.getMeanFourAvgBitsCenter() >= 0.75f)
                     && (sensor_bar.getMeanFourAvgBitsOuter() <= 0.25f)
                     && (delivery_counter < 4)) 
                    {
                        robot_state = RobotState::DELIVERY_ACTION;
                    }
                    break;
                case DELIVERY_ACTION:
                    // code for DELIVERY_ACTION state
                    stop_timer++;
                    motor_M1.setVelocity(0.0f);
                    motor_M2.setVelocity(0.0f);
                    if (stop_timer >= action_time * 1000 / main_task_period_ms) 
                    {
                        stop_timer = 0;
                        delivery_counter++;
                        robot_state = RobotState::DELIVERY_RESUME;
                    }
                    break;
                case DELIVERY_RESUME:
                    // code for DELIVERY_RESUME state
                    if (delivery_counter < NUMBER_OF_PICKUPS)
                    {
                        robot_state = RobotState::DELIVERY_APPROACH;
                    }
                    if (delivery_counter == NUMBER_OF_PICKUPS) 
                    {
                        robot_state = RobotState::APPROACH_END;
                    }
                    break;
                case EMERGENCY_STOP:
                    // code for EMERGENCY_STOP state
                    break;
                case APPROACH_END:
                    // code for APPROACH_END state
                    follow_line(angle, prev_angle, motor_M1, motor_M2, Crobot2wheel, robot_velocities, wheel_velocities, wheel_vel_max);
                    if ((sensor_bar.getMeanThreeAvgBitsLeft() >= 0.66f)
                        && (sensor_bar.getMeanThreeAvgBitsRight() <= 0.33f))
                        {
                            robot_state = RobotState::MISSION_END;
                        }
                    break;
                case MISSION_END:
                    // code for MISSION_END state
                    toggle_do_execute_main_fcn();
                    break;
            }
        } 
        else 
            {
            // the following code block gets executed only once
            if (do_reset_all_once) 
            {
                do_reset_all_once = false;

                // --- variables and objects that should be reset go here ---

                // reset variables and objects
                led1 = 0;
                enable_motors = 0; // enable motors
                robot_state = RobotState::INITIAL; // reset robot state
                pickup_counter = 0; // reset pickup counter
                delivery_counter = 0; // reset delivery counter
                stop_timer = 0; // reset stop timer
            }
        }

        // toggling the user led
        user_led = !user_led;

        printf("wheel speed M1: %f rps, wheel speed M2: %f rps\n", wheel_velocities(0) / (2.0f * M_PIf), wheel_velocities(1) / (2.0f * M_PIf));
        printf("max wheel speed M1: %f rps, max wheel speed M2: %f rps\n", motor_M1.getMaxVelocity(), motor_M2.getMaxVelocity());
        printf("sensor bar angle: %f rad, %f deg\n", angle, angle * 180.0f / M_PIf);
        printf("line sensor");

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