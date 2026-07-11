#include "mbed.h"
#include <map>

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

// color sensor
#include "ColorSensor.h"

// imu
#include "IMU.h"

// servo
#include "Servo.h"

#define M_PIf 3.14159265358979323846f // pi
#define MISSION_PERIOD 50 //
// robot params
#define WHEEL_DIAMETER 0.0665f // wheel diameter in meters
#define WHEEL_RADIUS (WHEEL_DIAMETER / 2.0f) // wheel radius in meters
#define WHEEL_BASE 0.153f // wheelbase, distance from wheel to wheel in meters
#define SENSOR_BAR_DISTANCE 0.037f // distance from wheel axis to leds on sensor bar / array in meters
#define VOLTAGE_MAX 12.0f // maximum voltage of battery packs, adjust this to 6.0f V if you only use one battery pack
#define GEAR_RATIO 100.0f
#define SPEED_CONSTANT 140.0f // speed constant of the motor
#define GEAR_RATION_RACK_PINION 390.63f
#define SPEED_CONSTANT_RACK_PINION 36.0f // speed constant of the motor with rack and pinion gear
#define KN_RACK_PINION (SPEED_CONSTANT_RACK_PINION / VOLTAGE_MAX) // [rad/s/V] speed constant of the motor with rack and pinion gear
#define KN (SPEED_CONSTANT / VOLTAGE_MAX) // [rad/s/V] speed constant of the motor
#define SPEED_FACTOR 0.15f // factor to reduce the speed of the robot, e.g. 0.1f means 10% of the maximum speed
#define PINION_PITCH 12e-3f // pitch of the pinion gear in meters
#define GAP_BETWEEN_MAGNETS 37e-3f
#define PACKAGE_HEIGHT 30e-3f
#define DISTANCE_TO_GROUND 34e-3f
// controller parameters
#define KP 1.2f // proportional gain for the rotational velocity controller
#define KD 0.26f // derivative gain for the rotational velocity controller
#define KP_NL 0.0f // proportional gain for the non-linear controller

// color parameters
#define RED 3
#define YELLOW 4
#define GREEN 5
#define BLUE 7
#define DELIVERY_CONFIRMATION_CYCLES 3 

//IMU parameters
#define PB_IMU_SDA PC_9
#define PB_IMU_SCL PA_8


struct PackagePosition 
{
    float height;
    float horizontal_offset;
};

std::map<int, PackagePosition> package_position_by_colour
{
    {RED,    {4e-3f + PACKAGE_HEIGHT,  25e-3f}},
    {BLUE,   {14e-3f  + PACKAGE_HEIGHT, 145e-3f}},
    {GREEN,  {4e-3f + PACKAGE_HEIGHT,  145e-3f}},
    {YELLOW, {14e-3f + PACKAGE_HEIGHT, 25e-3f}},
};
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
    robot_velocities = {SPEED_FACTOR * wheel_vel_max * WHEEL_RADIUS,
                        KP * angle + KP_NL * angle * fabs(angle) + KD * (angle - prev_angle)};

    wheel_velocities = Crobot2wheel * robot_velocities;

    motor_M1.setVelocity(wheel_velocities(0) / (2.0f * M_PIf));
    motor_M2.setVelocity(wheel_velocities(1) / (2.0f * M_PIf));
}

float distance_to_rotations(float distance)
{
    return (distance / (2 * M_PIf * PINION_PITCH));
}

float wheel_distance_to_rotations(float distance)
{
    return (distance / (2.0f * M_PIf * WHEEL_RADIUS));
}

bool is_color_already_detected(int color_num, const int detected_colors[], int detected_color_count)
{
    for (int i = 0; i < detected_color_count; i++)
    {
        if (detected_colors[i] == color_num)
        {
            return true;
        }
    }
    return false;
}

int findIndex(const int values[], int size, int target)
{
    for (int i = 0; i < size; i++) {
        if (values[i] == target) {
            return i;
        }
    }

    return -1; // not found
}
// main runs as an own thread
int main()
{

    
enum RobotState 
{
    INITIAL, 
    FIND_LINE,
    PICKUP_APPROACH,
    PICKUP_ALIGNMENT,
    PICKUP_ACTION,
    PICKUP_RESUME,
    DELIVERY_APPROACH,
    DELIVERY_ALIGNMENT,
    DELIVERY_ACTION,
    DELIVERY_RESUME,
    EMERGENCY_STOP,
    APPROACH_END,
    MISSION_END
} robot_state = RobotState::INITIAL;


enum ActionPhase
{
    MOVE_BACK,
    MOVE_BACK_STOP,
    LOWER_RACK,
    LOWER_RACK_STOP,
    RESTORE_RACK,
    RESTORE_RACK_STOP
} pickup_phase = ActionPhase::MOVE_BACK;

    // Delivery currently uses the same movement sequence as pickup.
    ActionPhase delivery_phase = ActionPhase::MOVE_BACK;

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

    // Motor setup
    DCMotor motor_M1(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, GEAR_RATIO, KN, VOLTAGE_MAX);
    DCMotor motor_M2(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, GEAR_RATIO, KN, VOLTAGE_MAX);
    DCMotor motor_M3(PB_PWM_M3, PB_ENC_A_M3, PB_ENC_B_M3, GEAR_RATION_RACK_PINION, KN_RACK_PINION, VOLTAGE_MAX);


    motor_M1.enableMotionPlanner();
    motor_M2.enableMotionPlanner();
    motor_M3.enableMotionPlanner();
    motor_M1.setMaxAcceleration(motor_M1.getMaxAcceleration() * 0.8f);
    motor_M2.setMaxAcceleration(motor_M2.getMaxAcceleration() * 0.8f);
    motor_M3.setMaxAcceleration(motor_M3.getMaxAcceleration() * 1.0f);
    motor_M3.setMaxVelocity(motor_M3.getMaxPhysicalVelocity() * 0.2);
    DigitalOut enable_motors(PB_ENABLE_DCMOTORS);
    
    const float wheel_vel_max = 2.0f * M_PIf * motor_M2.getMaxPhysicalVelocity();



    // sensor bar setup
    SensorBar sensor_bar(PB_9, PB_8, SENSOR_BAR_DISTANCE, true);
    float angle = 0.0f;


    // color sensor setup
    ColorSensor color_sensor(PB_3);
    int color_num;
    const char* color_string;
    color_sensor.switchLed(ON);

    // imu setup
    ImuData imu_data;
    IMU imu(PB_IMU_SDA, PB_IMU_SCL);
    

    // servo motors setup
    Servo servo_roll(PB_D0);
    Servo servo_pitch(PB_D1);

    float servo_ang_min = 0.035f;
    float servo_ang_max = 0.130f;

    servo_roll.calibratePulseMinMax(servo_ang_min, servo_ang_max);
    servo_pitch.calibratePulseMinMax(servo_ang_min, servo_ang_max);

    const float angle_range_min = -M_PIf / 2.0f; // -90 deg
    const float angle_range_max = M_PIf / 2.0f;  // 90 deg
    
    const float normalised_angle_gain = 1.0f / M_PIf; // normalized angle range is [-1, 1], therefore the gain is 1/pi
    const float normalised_angle_offset = 0.5f; // normalized angle range is [-1, 1], therefore the offset is 0

    static float roll_servo_width = 0.5f;
    static float pitch_servo_width = 0.5f;

    servo_roll.setPulseWidth(roll_servo_width);
    servo_pitch.setPulseWidth(pitch_servo_width);

    float rp[2] = {0.0f, 0.0f}; // roll, pitch



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
    float initial_rack_position = 48e-3f;
    int detected_colors[NUMBER_OF_PICKUPS] = {0, 0, 0, 0}; // colors detected so far, in order of detection
    int detected_color_count = 0;                          // how many unique colors detected so far
    int pickup_candidate_color = 0;
    int pickup_candidate_cycles = 0;
    int pickup_color_in_progress = 0;
    int delivered_colors[NUMBER_OF_PICKUPS] = {0, 0, 0, 0}; // delivery markers already serviced
    int delivered_color_count = 0;
    int delivery_candidate_color = 0;
    int delivery_candidate_cycles = 0;
    int delivery_color_in_progress = 0;

    float m1_rotation;
    float m2_rotation;
    float m3_rotation;

    // this loop will run forever
    while (true) 
    {
        main_task_timer.reset();

        // --- code that runs every cycle at the start goes here ---
        color_num = color_sensor.getColor();

        if (do_execute_main_task) 
        {

            // --- code that runs when the blue button was pressed goes here ---

            // visual feedback that the main task is executed, setting this once would actually be enough
            if (!servo_roll.isEnabled())
                servo_roll.enable();
            if (!servo_pitch.isEnabled())
                servo_pitch.enable();
        
            imu_data = imu.getImuData();

            rp[0] = imu_data.pry(1);
            rp[1] = imu_data.pry(0);

            roll_servo_width  = normalised_angle_gain * rp[0] + normalised_angle_offset;
            pitch_servo_width =  normalised_angle_gain * rp[1] + normalised_angle_offset;
            if (angle_range_min <= rp[0] && rp[0] <= angle_range_max)
                servo_roll.setPulseWidth(roll_servo_width);
            if (angle_range_min <= rp[1] && rp[1] <= angle_range_max)
                servo_pitch.setPulseWidth(pitch_servo_width);



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
                    motor_M3.setRotation(distance_to_rotations(initial_rack_position));

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

                    if ((color_num == RED) || (color_num == GREEN) || (color_num == BLUE) || (color_num == YELLOW))
                    {
                        if (!is_color_already_detected(color_num, detected_colors, detected_color_count)
                            && (detected_color_count < NUMBER_OF_PICKUPS))
                        {
                            // Confirm a new colour before starting pickup alignment.
                            if (color_num == pickup_candidate_color)
                            {
                                pickup_candidate_cycles++;
                            }
                            else
                            {
                                pickup_candidate_color = color_num;
                                pickup_candidate_cycles = 1;
                            }

                            if (pickup_candidate_cycles >= DELIVERY_CONFIRMATION_CYCLES)
                            {
                                pickup_color_in_progress = color_num;
                                pickup_candidate_color = 0;
                                pickup_candidate_cycles = 0;
                                robot_state = RobotState::PICKUP_ALIGNMENT;
                            }
                        }
                        else
                        {
                            pickup_candidate_color = 0;
                            pickup_candidate_cycles = 0;
                        }
                    }
                    else
                    {
                        pickup_candidate_color = 0;
                        pickup_candidate_cycles = 0;
                    }

                    break;
                case PICKUP_ALIGNMENT:
                    // code for PICKUP_ALIGNMENT state
                    follow_line(angle, prev_angle, motor_M1, motor_M2, Crobot2wheel, robot_velocities, wheel_velocities, wheel_vel_max);

                    if ((sensor_bar.getMeanFourAvgBitsCenter() >= 0.75f)
                        && (sensor_bar.getMeanFourAvgBitsOuter() >= 0.25f)
                    && (pickup_counter < 4)) 
                    {
                        motor_M1.setVelocity(0.0f);
                        motor_M2.setVelocity(0.0f);
                        robot_state = RobotState::PICKUP_ACTION;
                    }
                    break;
                case PICKUP_ACTION:
                    // code for PICKUP_ACTION state
                    switch (pickup_phase)
                    {
                        case MOVE_BACK: //move robot back
                            motor_M1.setMaxVelocity(motor_M1.getMaxPhysicalVelocity() * 0.2);
                            motor_M2.setMaxVelocity(motor_M2.getMaxPhysicalVelocity() * 0.2);
                            motor_M1.setRotation(motor_M1.getRotation() - wheel_distance_to_rotations(package_position_by_colour[pickup_color_in_progress].horizontal_offset 
                                + pickup_counter*GAP_BETWEEN_MAGNETS));
                            motor_M2.setRotation(motor_M2.getRotation() - wheel_distance_to_rotations(package_position_by_colour[pickup_color_in_progress].horizontal_offset 
                                + pickup_counter*GAP_BETWEEN_MAGNETS));
                            m1_rotation = motor_M1.getRotation();
                            m2_rotation = motor_M2.getRotation();
                            pickup_phase = MOVE_BACK_STOP;
                            break;
                        case MOVE_BACK_STOP: //condition to stop moving back
                            printf("phase 1\n");
                            if (fabs(motor_M1.getRotation() - (m1_rotation - wheel_distance_to_rotations(package_position_by_colour[pickup_color_in_progress].horizontal_offset + pickup_counter*GAP_BETWEEN_MAGNETS))) < 0.01f
                                && fabs(motor_M2.getRotation() - (m2_rotation - wheel_distance_to_rotations(package_position_by_colour[pickup_color_in_progress].horizontal_offset + pickup_counter*GAP_BETWEEN_MAGNETS))) < 0.01f)
                            {
                                motor_M1.setMaxVelocity(motor_M1.getMaxPhysicalVelocity());
                                motor_M2.setMaxVelocity(motor_M2.getMaxPhysicalVelocity());
                                pickup_phase = LOWER_RACK;
                            }
                            break;
                        case LOWER_RACK: //lower rack to specified position
                            printf("phase2\n");
                            motor_M3.setRotation(distance_to_rotations(package_position_by_colour[pickup_color_in_progress].height - DISTANCE_TO_GROUND));
                            m3_rotation = motor_M3.getRotation();
                            pickup_phase = LOWER_RACK_STOP;
                            break;
                        case LOWER_RACK_STOP: // condition to move to initial position
                            printf("phase 3\n");
                            if (fabs(motor_M3.getRotation() - (distance_to_rotations(package_position_by_colour[pickup_color_in_progress].height - DISTANCE_TO_GROUND)) < 0.01f))
                            {
                                pickup_phase = RESTORE_RACK;
                            }
                            break;
                        case RESTORE_RACK: // move to initial position after picking up
                            motor_M3.setRotation(distance_to_rotations(initial_rack_position));
                            pickup_phase = RESTORE_RACK_STOP;
                            printf("pickup phase 4\n");
                            break;
                        case RESTORE_RACK_STOP:  // condition to move to next phase
                            printf("pickup phase 5");
                            printf("condition = %f", fabs(motor_M3.getRotation() - distance_to_rotations(initial_rack_position)) < 0.001f);
                            if (fabs(motor_M3.getRotation() - distance_to_rotations(initial_rack_position)) < 0.001f)
                            {
                                if (detected_color_count < NUMBER_OF_PICKUPS)
                                {
                                    detected_colors[detected_color_count] = pickup_color_in_progress;
                                    detected_color_count++;
                                }
                                pickup_color_in_progress = 0;
                                pickup_phase = MOVE_BACK;
                                pickup_counter++;
                                robot_state = RobotState::PICKUP_RESUME;
                            }
                            break;
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
                    follow_line(angle, prev_angle, motor_M1, motor_M2, Crobot2wheel, robot_velocities, wheel_velocities, wheel_vel_max);

                    // Require the same new colour for consecutive control cycles before
                    // stopping. This rejects one-cycle colour-sensor noise.
                    if ((color_num == RED) || (color_num == YELLOW) ||
                        (color_num == GREEN) || (color_num == BLUE))
                    {
                        if (!is_color_already_detected(color_num, delivered_colors, delivered_color_count))
                        {
                            if (color_num == delivery_candidate_color)
                            {
                                delivery_candidate_cycles++;
                            }
                            else
                            {
                                delivery_candidate_color = color_num;
                                delivery_candidate_cycles = 1;
                            }

                            if (delivery_candidate_cycles >= DELIVERY_CONFIRMATION_CYCLES)
                            {
                                delivery_color_in_progress = color_num;
                                delivery_candidate_color = 0;
                                delivery_candidate_cycles = 0;
                                robot_state = RobotState::DELIVERY_ALIGNMENT;
                            }
                        }
                        else
                        {
                            // Ignore a marker that was already delivered.
                            delivery_candidate_color = 0;
                            delivery_candidate_cycles = 0;
                        }
                    }
                    else
                    {
                        delivery_candidate_color = 0;
                        delivery_candidate_cycles = 0;
                    }
                    break;
                case DELIVERY_ALIGNMENT:
                    // Match the pickup alignment behaviour before operating the rack.
                    follow_line(angle, prev_angle, motor_M1, motor_M2, Crobot2wheel, robot_velocities, wheel_velocities, wheel_vel_max);

                    if ((sensor_bar.getMeanFourAvgBitsCenter() >= 0.75f)
                        && (sensor_bar.getMeanFourAvgBitsOuter() <= 0.25f)
                        && (delivery_counter < NUMBER_OF_PICKUPS))
                    {
                        motor_M1.setVelocity(0.0f);
                        motor_M2.setVelocity(0.0f);
                        robot_state = RobotState::DELIVERY_ACTION;
                    }
                    break;
                case DELIVERY_ACTION:
                    // Use the same move-back, lower-rack, restore-rack sequence as pickup.
                    switch (delivery_phase)
                    {
                        case MOVE_BACK:
                            motor_M1.setMaxVelocity(motor_M1.getMaxPhysicalVelocity() * 0.2);
                            motor_M2.setMaxVelocity(motor_M2.getMaxPhysicalVelocity() * 0.2);
                            motor_M1.setRotation(motor_M1.getRotation() - wheel_distance_to_rotations(package_position_by_colour[delivery_color_in_progress].horizontal_offset + 
                            findIndex(detected_colors, NUMBER_OF_PICKUPS, delivery_color_in_progress) * GAP_BETWEEN_MAGNETS));
                            motor_M2.setRotation(motor_M2.getRotation() - wheel_distance_to_rotations(package_position_by_colour[delivery_color_in_progress].horizontal_offset +
                            findIndex(detected_colors, NUMBER_OF_PICKUPS, delivery_color_in_progress) * GAP_BETWEEN_MAGNETS));
                            m1_rotation = motor_M1.getRotation();
                            m2_rotation = motor_M2.getRotation();
                            delivery_phase = MOVE_BACK_STOP;
                            break;
                        case MOVE_BACK_STOP:
                            if (fabs(motor_M1.getRotation() - (m1_rotation - wheel_distance_to_rotations(package_position_by_colour[delivery_color_in_progress].horizontal_offset + 
                            findIndex(detected_colors, NUMBER_OF_PICKUPS, delivery_color_in_progress) * GAP_BETWEEN_MAGNETS))) < 0.01f
                                && fabs(motor_M2.getRotation() - (m2_rotation - wheel_distance_to_rotations(package_position_by_colour[delivery_color_in_progress].horizontal_offset +
                                findIndex(detected_colors, NUMBER_OF_PICKUPS, delivery_color_in_progress) * GAP_BETWEEN_MAGNETS))) < 0.01f)
                            {
                                motor_M1.setMaxVelocity(motor_M1.getMaxPhysicalVelocity());
                                motor_M2.setMaxVelocity(motor_M2.getMaxPhysicalVelocity());
                                delivery_phase = LOWER_RACK;
                            }
                            break;
                        case LOWER_RACK:
                            motor_M3.setRotation(distance_to_rotations(package_position_by_colour[delivery_color_in_progress].height - DISTANCE_TO_GROUND));
                            m3_rotation = motor_M3.getRotation();
                            delivery_phase = LOWER_RACK_STOP;
                            break;
                        case LOWER_RACK_STOP:
                            if (fabs(motor_M3.getRotation() - (distance_to_rotations(package_position_by_colour[delivery_color_in_progress].height -DISTANCE_TO_GROUND))) < 0.01f)
                            {
                                delivery_phase = RESTORE_RACK;
                            }
                            break;
                        case RESTORE_RACK:
                            motor_M3.setRotation(distance_to_rotations(initial_rack_position));
                            delivery_phase = RESTORE_RACK_STOP;
                            break;
                        case RESTORE_RACK_STOP:
                            if (fabs(motor_M3.getRotation() - distance_to_rotations(initial_rack_position)) < 0.001f)
                            {
                                // Commit the marker only after its delivery action completes.
                                if (delivered_color_count < NUMBER_OF_PICKUPS)
                                {
                                    delivered_colors[delivered_color_count] = delivery_color_in_progress;
                                    delivered_color_count++;
                                }
                                delivery_color_in_progress = 0;
                                delivery_phase = MOVE_BACK;
                                delivery_counter++;
                                robot_state = RobotState::DELIVERY_RESUME;
                            }
                            break;
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
                robot_state = RobotState::INITIAL; // reset robot state
                pickup_counter = 0; // reset pickup counter
                delivery_counter = 0; // reset delivery counter
                pickup_phase = ActionPhase::MOVE_BACK;
                delivery_phase = ActionPhase::MOVE_BACK;
                roll_servo_width = 0.5f;
                pitch_servo_width = 0.5f;
                servo_roll.setPulseWidth(roll_servo_width);
                servo_pitch.setPulseWidth(pitch_servo_width);
                motor_M1.setVelocity(0.0f);
                motor_M2.setVelocity(0.0f);
                motor_M3.setRotation(0.0f);
                detected_colors[0] = 0;
                detected_colors[1] = 0;
                detected_colors[2] = 0;
                detected_colors[3] = 0;
                detected_color_count = 0;
                pickup_candidate_color = 0;
                pickup_candidate_cycles = 0;
                pickup_color_in_progress = 0;
                delivered_colors[0] = 0;
                delivered_colors[1] = 0;
                delivered_colors[2] = 0;
                delivered_colors[3] = 0;
                delivered_color_count = 0;
                delivery_candidate_color = 0;
                delivery_candidate_cycles = 0;
                delivery_color_in_progress = 0;

            }
        }

        // toggling the user led
        user_led = !user_led;

        color_string = color_sensor.getColorString(color_num);
        // printf("Detected color: %s\n Color Number: %d\n", color_string, color_num);

        // printf("wheel speed M1: %f rps, wheel speed M2: %f rps\n", wheel_velocities(0) / (2.0f * M_PIf), wheel_velocities(1) / (2.0f * M_PIf));
        // // printf("max wheel speed M1: %f rps, max wheel speed M2: %f rps\n", motor_M1.getMaxVelocity(), motor_M2.getMaxVelocity());
        // printf("rack rotations: %f", motor_M3.getRotation());
        // printf("sensor bar angle: %f rad, %f deg\n", angle, angle * 180.0f / M_PIf);
        // printf("Averaged Bar Raw: |  %0.2f  | %0.2f |  %0.2f |  %0.2f |  %0.2f |  %0.2f |  %0.2f |  %0.2f | ", sensor_bar.getAvgBit(0)
        //                                                                                              , sensor_bar.getAvgBit(1)
        //                                                                                              , sensor_bar.getAvgBit(2)
        //                                                                                              , sensor_bar.getAvgBit(3)
        //                                                                                              , sensor_bar.getAvgBit(4)
        //                                                                                              , sensor_bar.getAvgBit(5)
        //                                                                                              , sensor_bar.getAvgBit(6)
        //                                                                                              , sensor_bar.getAvgBit(7));

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
