#include "mbed.h"

/**
 * ToDo:
 * - remove precalc and storage from GPA
 * - check if additional size of main is after still necessary
 * - adapt logik from https://www.geeksforgeeks.org/snprintf-c-library/ to GPA
 * - implement rotation logik for different mounted board in IMU
*/

#include "pm2_drivers/DebounceIn.h"
#include "pm2_drivers/Servo.h"
#include "pm2_drivers/EncoderCounter.h"
#include "pm2_drivers/DCMotor.h"
#include "pm2_drivers/UltrasonicSensor.h"
#include "pm2_drivers/IMU.h"

#define NEW_PES_BOARD_VERSION
#ifdef NEW_PES_BOARD_VERSION
    #define USER_BUTTON PC_13
    #define USER_LED PA_5

    #define PB_D0 PB_2
    #define PB_D1 PC_8
    #define PB_D2 PC_6
    #define PB_D3 PB_12

    #define PB_PWM_M1 PB_13
    #define PB_PWM_M2 PA_9
    #define PB_PWM_M3 PA_10

    #define PB_ENC_A_M1 PA_6
    #define PB_ENC_B_M1 PC_7
    #define PB_ENC_A_M2 PB_6
    #define PB_ENC_B_M2 PB_7
    #define PB_ENC_A_M3 PA_0
    #define PB_ENC_B_M3 PA_1

    #define PB_IMU_SDA PC_9
    #define PB_IMU_SCL PA_8

    #define PN_ENABLE_DCMOTORS PB_15
#else
    #define USER_BUTTON PC_13
    #define USER_LED PA_5

    #define PB_D0 PC_9  // ???
    #define PB_D1 PC_8  // ???
    #define PB_D2 PC_6  // ???
    #define PB_D3 PB_12 // ???

    #define PB_PWM_M1 PA_8
    #define PB_PWM_M2 PA_9
    #define PB_PWM_M3 PA_13 // ???

    #define PB_ENC_A_M1 PB_6
    #define PB_ENC_B_M1 PB_7
    #define PB_ENC_A_M2 PA_6
    #define PB_ENC_A_M2 PC_7
    #define PB_ENC_A_M3 PA_1 // ???
    #define PB_ENC_B_M3 PA_0 // ???

    #define PB_IMU_SDA // ???
    #define PB_IMU_SCL // ???
    
    #define PN_ENABLE_DCMOTORS PB_2 // PB_13 ???
#endif

bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(USER_BUTTON);  // create InterruptIn interface object to evaluate user button falling and
                                      // rising edge (no blocking code in ISR)
void user_button_pressed_fcn();       // custom functions which get executed when user
                                      // button gets pressed, definition below

// main runs as an own thread
int main()
{
    enum RobotState {
        INIT = 0,
        FORWARD,
        BACKWARD,
        SLEEP,
    } robot_state = RobotState::INIT;

    // attach button fall function to user button object, button has a pull-up resistor
    user_button.fall(&user_button_pressed_fcn);

    // while loop gets executed every main_task_period_ms milliseconds (simple
    // aproach to repeatedly execute main)
    const int main_task_period_ms = 20; // define main task period time in ms e.g. 50 ms
                                        // -> main task runs 20 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task every main_task_period_ms
    Timer timer;

    // led on nucleo board
    // create DigitalOut object to command user led
    DigitalOut user_led(USER_LED);

    // additional led's
    // create DigitalOut object to command extra led (you need to add an aditional
    // resistor, e.g. 220...500 Ohm) an led as an anode (+) and a cathode (-), the
    // cathode is connected to ground via a resistor
    DigitalOut led1(PB_8);
    DigitalOut led2(PB_9);

    // mechanical button
    DigitalIn mechanical_button(PC_5); // create DigitalIn object to evaluate extra mechanical button, you
                                       // need to specify the mode for proper usage, see below
    mechanical_button.mode(PullUp);    // set pullup mode: sets pullup between pin and 3.3 V, so that there
                                       // is a defined potential

    // Sharp GP2Y0A41SK0F, 4-40 cm IR Sensor
    float ir_distance_mV = 0.0f; // define variable to store measurement
    AnalogIn ir_analog_in(PC_2); // create AnalogIn object to read in infrared distance sensor, 0...3.3V are mapped to 0...1

    // create Servo objects to command servos
    Servo servo_D0(PB_D0);
    Servo servo_D1(PB_D1);
    Servo servo_D2(PB_D2);

    // Those variables should be filled out with values obtained in the
    // calibration process, these are minimal pulse width and maximal pulse width.
    //  Futuba S3001
    float servo_D0_ang_min = 0.0150f;
    float servo_D0_ang_max = 0.1150f;
    // Modelcraft RS2 MG/BB
    float servo_D1_ang_min = 0.0325f;
    float servo_D1_ang_max = 0.1250f;
    // REELY S0090
    float servo_D2_ang_min = 0.0325f;
    float servo_D2_ang_max = 0.1175f;

    // setNormalisedPulseWidth: before calibration (0,1) -> (min pwm, max pwm)
    servo_D0.calibratePulseMinMax(servo_D0_ang_min, servo_D0_ang_max);
    servo_D1.calibratePulseMinMax(servo_D1_ang_min, servo_D1_ang_max);
    servo_D2.calibratePulseMinMax(servo_D2_ang_min, servo_D2_ang_max);
    // setNormalisedPulseWidth: after calibration (0,1) -> (servo_D0_ang_min, servo_D0_ang_max)

    // default is 1.0e6f, this is the default acceleration for the motion profile,
    // this is the maximum acceleration of the servo
    servo_D0.setMaxAcceleration(3.0f);
    servo_D1.setMaxAcceleration(3.0f);
    servo_D2.setMaxAcceleration(3.0f);
    // after this there is smooth movement

    float servo_angle = 0; // servo S1 normalized input: 0...1
    int servo_counter = 0; // define servo counter, this is an additional variable
                           // to make the servos move
    const int loops_per_seconds = static_cast<int>(ceilf(1.0f / (0.001f * (float)main_task_period_ms)));

    // 78:1, 100:1, ... Metal Gearmotor 20Dx44L mm 12V CB
    // motor M1 is used open-loop
    // motor M2 is used closed-loop to command velocity (rotations per second)
    // motor M3 is used closed-loop to command position (rotations)
    DigitalOut enable_motors(PN_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors

    // FastPWM pwm_M1(PB_PWM_M1);                           // create FastPWM object to command motor M1
    // EncoderCounter encoder_M1(PB_ENC_A_M1, PB_ENC_B_M1); // create EncoderCounter object to read in the encoder counter
    //                                                      // values, since M1 is used open-loop no encoder would be
    //                                                      // needed for operation, this is just an example

    const float voltage_max = 12.0f;     // maximum voltage of battery packs, adjust this to 6.0f V if you only use one batterypack

    // https://www.pololu.com/product/3475/specs
    const float gear_ratio_M1 = 31.25f;                        // 31.25:1 gear box
    const float kn_M1 = 450.0f / 12.0f;                        // motor constant in RPM / V
    DCMotor motor_M1(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio_M1, kn_M1, voltage_max);
    const float velocity_max_M1 = kn_M1 * voltage_max / 60.0f; // maximum velocity in rotations per second
    motor_M1.setMaxVelocity(velocity_max_M1 * 0.5f);           // set maximum velocity to 80% of maximum velocity
    
    // https://www.pololu.com/product/3485/specs
    const float gear_ratio_M2 = 488.28125f;                    // 488.28125:1 gear box
    const float kn_M2 = 28.0f / 12.0f;                         // motor constant in RPM / V
    DCMotor motor_M2(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, gear_ratio_M2, kn_M2, voltage_max);
    const float velocity_max_M2 = kn_M2 * voltage_max / 60.0f; // maximum velocity in rotations per second
    motor_M2.setMaxVelocity(velocity_max_M2 * 0.5f);           // set maximum velocity to 80% of maximum velocity

    // https://www.pololu.com/product/3477/specs
    const float gear_ratio_M3 = 78.125f;                       // 78.125:1 gear box
    const float kn_M3 = 180.0f / 12.0f;                        // motor constant in RPM / V
    DCMotor motor_M3(PB_PWM_M3, PB_ENC_A_M3, PB_ENC_B_M3, gear_ratio_M3, kn_M3, voltage_max);
    const float velocity_max_M3 = kn_M3 * voltage_max / 60.0f; // maximum velocity in rotations per second
    motor_M3.setMaxVelocity(velocity_max_M3 * 0.5f);           // set maximum velocity to 80% of maximum velocity

    const float motor_setpoint_M1 = 300.0f / gear_ratio_M1;
    const float motor_setpoint_M2 = 300.0f / gear_ratio_M2;
    const float motor_setpoint_M3 = 300.0f / gear_ratio_M3;
    

    // Groove Ultrasonic Ranger V2.0
    float us_distance_cm = 0.0f;    // define variable to store measurement
    UltrasonicSensor us_sensor(PB_D3);

    // IMU
    ImuData imu_data;
    IMU imu(PB_IMU_SDA, PB_IMU_SCL);    

    // start timer
    main_task_timer.start();
    timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        if (do_execute_main_task) {
            // read analog input
            ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;

            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;

            // read ultra sonic distance sensor
            us_distance_cm = us_sensor.read();

            // read imu data
            imu_data = imu.getImuData();

            // commanding the servos
            if (servo_D0.isEnabled() && servo_D1.isEnabled() && servo_D2.isEnabled()) {
                // command servo position, increment normalised angle every second until
                // it reaches 1.0f servo_D0.setNormalisedPulseWidth(servo_angle);
                // servo_D1.setNormalisedPulseWidth(servo_angle);
                // servo_D2.setNormalisedPulseWidth(servo_angle);
                /**
                if (servo_angle < 1.0f & servo_counter % loops_per_seconds == 0 &
                servo_counter != 0)
                {
                    //servo_angle += 0.0025f;
                    //servo_angle += 0.1f;
                    servo_angle = 1.0f;

                }
                servo_counter++;
                */

                // servo_D0.setNormalisedPulseWidth(servo_D0_ang_max);
                // servo_D1.setNormalisedPulseWidth(servo_D1_ang_max);
                // servo_D2.setNormalisedPulseWidth(servo_D2_ang_max);
                servo_D0.setNormalisedPulseWidth(0.0f);
                servo_D1.setNormalisedPulseWidth(0.0f);
                servo_D2.setNormalisedPulseWidth(0.0f);
            }

            // state machine
            switch (robot_state) {
                case RobotState::INIT:
                    // enable servos if not enabled
                    if (!servo_D0.isEnabled())
                        servo_D0.enable();
                    if (!servo_D1.isEnabled())
                        servo_D1.enable();
                    if (!servo_D2.isEnabled())
                        servo_D2.enable();

                    // enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled
                    enable_motors = 1;
                    
                    motor_M1.setRotation(0.0f);
                    motor_M2.setRotation(0.0f);
                    motor_M3.setRotation(0.0f);

                    robot_state = RobotState::FORWARD;
                    break;

                case RobotState::FORWARD:
                    if (mechanical_button.read()) {
                        led2 = 1;

                        // pwm_M1.write(0.75f);          // write output to motor M1

                        motor_M1.setRotation(motor_setpoint_M1);
                        motor_M2.setRotation(motor_setpoint_M2);
                        motor_M3.setRotation(motor_setpoint_M3);

                        robot_state = RobotState::BACKWARD;
                    }
                    break;

                case RobotState::BACKWARD:
                    if (motor_M3.getRotation() >= 0.95f * motor_setpoint_M3) {
                        // pwm_M1.write(0.25f);

                        motor_M1.setRotation(0.0f);
                        motor_M2.setRotation(0.0f);
                        motor_M3.setRotation(0.0f);
                        
                        robot_state = RobotState::SLEEP;
                    }
                    break;

                case RobotState::SLEEP:

                    if (motor_M3.getRotation() <= 0.05f * motor_setpoint_M3) {
                        // enable_motors = 0;

                        // pwm_M1.write(0.5f);
                      
                        motor_M1.setRotation(0.0f);
                        motor_M2.setRotation(0.0f);
                        motor_M3.setRotation(0.0f);
                      
                        // robot_state is not changed, there for the state machine remains in here until the blue button is pressed again
                    }

                default:
                    break; // do nothing
                }
        } else {
            if (do_reset_all_once) {
                // toggle do_reset_all_once to make sure this code is only executed once
                do_reset_all_once = false;

                ir_distance_mV = 0.0f;

                us_distance_cm = 0.0f;

                imu_data.initialise();

                // pwm_M1.write(0.5f);
                // motor_M2.setVelocity(0.0f);
                // motor_M3.setRotation(0.0f);

                servo_angle = 0.0f;
                // servo_D0.setNormalisedPulseWidth(servo_angle);
                // servo_D1.setNormalisedPulseWidth(servo_angle);
                // servo_D2.setNormalisedPulseWidth(servo_angle);
                // servo_D0.setNormalisedPulseWidth(servo_D0_ang_min);
                // servo_D1.setNormalisedPulseWidth(servo_D1_ang_min);
                // servo_D2.setNormalisedPulseWidth(servo_D2_ang_min);
                servo_D0.setNormalisedPulseWidth(1.0f);
                servo_D1.setNormalisedPulseWidth(1.0f);
                servo_D2.setNormalisedPulseWidth(1.0f);

                led1 = led2 = 0;

                robot_state = RobotState::INIT;
            }
        }

        // toggling the user led
        user_led = !user_led;

        // do only output via serial what's really necessary, this makes your code
        // slow printf("IR sensor (mV): %3.3f, Encoder M1: %3d, Speed M2 (rps)
        // %3.3f, Position M3 (rot): %3.3f, Servo S1 angle (normalized): %3.4f,
        // Servo S2 angle (normalized): %3.4f\r\n",
        //        ir_distance_mV,
        //        encoder_M1.read(),
        //        speedController_M2.getSpeedRPS(),
        //        motor_M3.getRotation(),
        //        servo_angle,
        //        servo_D1_angle);
        // printf("%f\n", servo_angle);

        int time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count();
        DCMotor* DCMotor_ptr = &motor_M1;
        printf("%d, %d, %f, %f, %f, %f, %f, %f, %f, %f\n",
               time_ms,
               robot_state,
               DCMotor_ptr->getRotationTarget(),
               DCMotor_ptr->getRotationSetpoint(),
               DCMotor_ptr->getRotation(),
               DCMotor_ptr->getVelocityTarget(),
               DCMotor_ptr->getVelocitySetpoint(),
               DCMotor_ptr->getVelocity(),
               DCMotor_ptr->getVoltage(),
               DCMotor_ptr->getPWM());

        // printf("%f\n", us_distance_cm);

        // float time_ms = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count() * 1.0e-3f;
        // printf("%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, ", imu_data.gyro(0), imu_data.gyro(1), imu_data.gyro(2),
        //                                                                        imu_data.acc(0) , imu_data.acc(1) , imu_data.acc(2) ,
        //                                                                        imu_data.mag(0) , imu_data.mag(1) , imu_data.mag(2) , time_ms );
        // printf("%.6f, %.6f, %.6f, %.6f, ", imu_data.quat.w(), imu_data.quat.x(), imu_data.quat.y(), imu_data.quat.z() );
        // printf("%.6f, %.6f, %.6f, ", imu_data.rpy(0), imu_data.rpy(1), imu_data.rpy(2) );
        // printf("%.6f\n", imu_data.tilt);

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

void user_button_pressed_fcn()
{
    // do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    if (do_execute_main_task)
        do_reset_all_once = true;
}