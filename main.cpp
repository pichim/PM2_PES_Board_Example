#include "mbed.h"

// pes board pin map
#include "pm2_drivers/PESBoardPinMap.h"

// drivers
#include "pm2_drivers/DebounceIn.h"
#include "pm2_drivers/IMU.h"
#include "pm2_drivers/Servo.h"
#include <complex>

bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once
// while loop gets executed every main_task_period_ms milliseconds, this is a
// simple approach to repeatedly execute main
const int main_task_period_ms = 20; // define main task period time in ms e.g. 20 ms, there for
                                    // the main task will run 50 times per second

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(USER_BUTTON); // create DebounceIn object to evaluate the user button
                                     // falling and rising edge
void toggle_do_execute_main_fcn();   // custom function which is getting executed when user
                                     // button gets pressed, definition below

// main runs as an own thread
int main()
{
    // attach button fall function address to user button object, button has a pull-up resistor
    user_button.fall(&toggle_do_execute_main_fcn);

    Timer main_task_timer;              // create Timer object which we use to run the main task
                                        // every main_task_period_ms
    Timer timer;

    // led on nucleo board
    DigitalOut user_led(USER_LED);

    // servo
    Servo servo_roll(PB_D0);
    Servo servo_pitch(PB_D1);

    // IMU
    ImuData imu_data;
    IMU imu(PB_IMU_SDA, PB_IMU_SCL);    
    Eigen::Vector2f rp;

    // minimal pulse width and maximal pulse width obtained from the servo calibration process
    // nodelcraft RS2 MG/BB
    const float servo_ang_min = 0.03f; // carefull, these values might differ from servo to servo
    const float servo_ang_max = 0.13f;

    // servo.setNormalisedPulseWidth: before calibration (0,1) -> (min pwm, max pwm)
    // servo.setNormalisedPulseWidth: after calibration (0,1) -> (servo_D0_ang_min, servo_D0_ang_max)
    servo_roll.calibratePulseMinMax(servo_ang_min, servo_ang_max);
    servo_pitch.calibratePulseMinMax(servo_ang_min, servo_ang_max);

    // angle limits of the servos
    const float angle_range_min = -M_PI/2.0f;
    const float angle_range_max = M_PI/2.0f;    

    // angle to pulse width coefficients
    const float normalised_angle_gain = 1.0f / M_PI;
    const float normalised_angle_offset = 0.5f;

    // pulse width
    static float roll_servo_width = 0.5f;
    static float pitch_servo_width = 0.5f;

    servo_roll.setNormalisedPulseWidth(roll_servo_width);
    servo_pitch.setNormalisedPulseWidth(pitch_servo_width);

    // start timer
    main_task_timer.start();
    timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        if (do_execute_main_task) {
            
            if (!servo_roll.isEnabled())
                servo_roll.enable();
            if (!servo_pitch.isEnabled())
                servo_pitch.enable();

            imu_data = imu.getImuData();

            // roll
            rp(0) = atan2f(imu_data.quat.x() + imu_data.quat.z(), imu_data.quat.w() - imu_data.quat.y()) - atan2f(imu_data.quat.z() - imu_data.quat.x(), imu_data.quat.y() + imu_data.quat.w());
            // pitch
            rp(1) = acosf((imu_data.quat.w() - imu_data.quat.y()) * (imu_data.quat.w() - imu_data.quat.y()) + (imu_data.quat.x() + imu_data.quat.z()) * (imu_data.quat.x() + imu_data.quat.z()) - 1.0f) - M_PI / 2.0f;

            roll_servo_width = -normalised_angle_gain * rp(0) + normalised_angle_offset;
            pitch_servo_width = normalised_angle_gain * rp(1) + normalised_angle_offset;

            if (rp(0) < angle_range_max && rp(0) > angle_range_min) {
                servo_roll.setNormalisedPulseWidth(roll_servo_width);
            }
            if (rp(0) < angle_range_max && rp(0) > angle_range_min) {
                servo_pitch.setNormalisedPulseWidth(pitch_servo_width);    
            }

        } else {
            // the following code block gets executed only once
            if (do_reset_all_once) {
                do_reset_all_once = false;
                roll_servo_width = 0.5f;
                pitch_servo_width = 0.5f;
                servo_roll.setNormalisedPulseWidth(roll_servo_width);
                servo_pitch.setNormalisedPulseWidth(pitch_servo_width);
            }
        }

        // toggling the user led
        user_led = !user_led;

        float time_ms = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count() * 1.0e-3f;

        printf("Roll: %f, Pitch: %f \n", roll_servo_width, pitch_servo_width);
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
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