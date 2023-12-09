#include "mbed.h"

#include "pm2_drivers/IMUThread.h"
#include "pm2_drivers/LinearCharacteristics.h"
#include "pm2_drivers/LinearCharacteristics3.h"
#include "pm2_drivers/Mahony.h"
#include "pm2_drivers/PM2_Drivers.h"

// PES-Board Pin Names
#define PB_D0 PB_2
#define PB_D1 PC_8
#define PB_D2 PC_6
#define PB_D3 PB_12

#define PB_M1_PWM PB_13
#define PB_M2_PWM PA_9
#define PB_M3_PWM PA_10

#define PB_M1_ENC_A PA_6
#define PB_M1_ENC_B PC_7
#define PB_M2_ENC_A PB_6
#define PB_M2_ENC_B PB_7
#define PB_M3_ENC_A PA_0
#define PB_M3_ENC_B PA_1

// #include "pm2_drivers/PESBoardPinName.h"

bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false; // this variable is used to reset certain variables and objects and
                                // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(PC_13);  // create InterruptIn interface object to evaluate user button falling and rising edge (no blocking code in ISR)
void user_button_pressed_fcn(); // custom functions which get executed when user
                                // button gets pressed, definition below

// main runs as an own thread
int main()
{
    // Mutex imuMutex;
    // Data data;
    // IMUThread imuThread(data, imuMutex);
    // imuThread.StartThread();

    enum RobotState {
        INIT,
        FORWARD,
        BACKWARD,
        SLEEP,
    };
    RobotState robot_state = RobotState::INIT;

    // attach button fall function to user button object, button has a pull-up resistor
    user_button.fall(&user_button_pressed_fcn);

    // while loop gets executed every main_task_period_ms milliseconds (simple
    // aproach to repeatedly execute main)
    const int main_task_period_ms = 10; // define main task period time in ms e.g. 50 ms -> main task runs 20 times per second
    Timer main_task_timer; // create Timer object which we use to run the main task every main_task_period_ms
    Timer timer;

    // led on nucleo board
    // create DigitalOut object to command user led
    DigitalOut user_led(LED1);

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

    // // create Servo objects to command servos
    // Servo servo_D0(PB_D0);
    // Servo servo_D1(PB_D1);
    // Servo servo_D2(PB_D2);

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

    // // setNormalisedPulseWidth: before calibration (0,1) -> (min pwm, max pwm)
    // servo_D0.calibratePulseMinMax(servo_D0_ang_min, servo_D0_ang_max);
    // servo_D1.calibratePulseMinMax(servo_D1_ang_min, servo_D1_ang_max);
    // servo_D2.calibratePulseMinMax(servo_D2_ang_min, servo_D2_ang_max);
    // // setNormalisedPulseWidth: after calibration (0,1) -> (servo_D0_ang_min, servo_D0_ang_max)

    // // default is 1.0e6f, this is the default acceleration for the motion profile,
    // // this is the maximum acceleration of the servo
    // servo_D0.setMaxAcceleration(3.0f);
    // servo_D1.setMaxAcceleration(3.0f);
    // servo_D2.setMaxAcceleration(3.0f);
    // // after this there is smooth movement

    float servo_angle = 0; // servo S1 normalized input: 0...1
    int servo_counter = 0; // define servo counter, this is an additional variable
                           // to make the servos move
    const int loops_per_seconds = static_cast<int>(ceilf(1.0f / (0.001f * (float)main_task_period_ms)));

    // 78:1, 100:1, ... Metal Gearmotor 20Dx44L mm 12V CB
    // - motor M1 is used open-loop
    // - motor M2 is closed-loop speed controlled (rotations per second)
    // - motor M3 is closed-loop position controlled (rotations)
    DigitalOut enable_motors(PB_15); // create DigitalOut object to enable dc motors

    FastPWM pwm_M1(PB_M1_PWM); // create FastPWM object to command motor M1
    EncoderCounter encoder_M1(PB_M1_ENC_A, PB_M1_ENC_B); // create EncoderCounter object to read in the encoder counter
                                                         // values, since M1 is used open-loop no encoder would be
                                                         // needed for operation, this is just an example

    // create SpeedController and DCMotor objects, default parametrization is for 78.125:1 gear box
    const float gear_ratio = 78.125f;                 // 78.125:1 gear box
    const float voltage_max = 12.0f;                  // define maximum voltage of battery packs, adjust this to 6.0f V if you only use one batterypack
    const float counts_per_turn = 20.0f * gear_ratio; // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn = 180.0f / 12.0f;                  // define motor constant in RPM / V

    // const float k_gear = 100.0f / gear_ratio;         // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    // const float kp = 0.2f;                            // define custom kp, this is the default speed 1controller gain for gear box 78.125:1

    // SpeedController speedController_M2(counts_per_turn, kn, voltage_max, pwm_M2, encoder_M2); // default 78.125:1 gear box  with default contoller parameters
    // SpeedController speedController_M2(counts_per_turn * k_gear, kn / k_gear, voltage_max, pwm_M2, encoder_M2); // parameters adjusted to 100:1 gear

    DCMotor DCMotor_M2(PB_M2_PWM, PB_M2_ENC_A, PB_M2_ENC_B, counts_per_turn, kn, voltage_max);
    DCMotor DCMotor_M3(PB_M3_PWM, PB_M3_ENC_A, PB_M3_ENC_B, counts_per_turn, kn, voltage_max);
    DCMotor_M3.setMaxVelocity(1.5f);
    // DCMotor DCMotor_M3(PB_M3_PWM, PB_M3_ENC_A, PB_M3_ENC_B, counts_per_turn * k_gear, kn / k_gear, voltage_max); // parameters adjusted to 100:1 gear, we
    // need a different speed controller gain here
    // DCMotor_M3.setVelocityCntrlGain(kp * k_gear); // adjust internal speed
    // controller gain, this is just an example
    float max_speed_rps = 0.5f; // define maximum speed that the position controller is changig the
                                // speed, has to be smaller or equal to kn * voltage_max
    // DCMotor_M3.setMaxVelocity(max_speed_rps); // adjust max velocity for internal speed controller

    main_task_timer.start();
    timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        if (do_execute_main_task) {
            // read analog input
            ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;

            // visual feedback that the main task is executed, setting this once would
            // actually be enough
            led1 = 1;

            // // commanding the servos
            // if (servo_D0.isEnabled() && servo_D1.isEnabled() && servo_D2.isEnabled()) {
            //     // command servo position, increment normalised angle every second until
            //     // it reaches 1.0f servo_D0.setNormalisedPulseWidth(servo_angle);
            //     // servo_D1.setNormalisedPulseWidth(servo_angle);
            //     // servo_D2.setNormalisedPulseWidth(servo_angle);
            //     /**
            //     if (servo_angle < 1.0f & servo_counter % loops_per_seconds == 0 &
            //     servo_counter != 0)
            //     {
            //         //servo_angle += 0.0025f;
            //         //servo_angle += 0.1f;
            //         servo_angle = 1.0f;

            //     }
            //     servo_counter++;
            //     */

            //     // servo_D0.setNormalisedPulseWidth(servo_D0_ang_max);
            //     // servo_D1.setNormalisedPulseWidth(servo_D1_ang_max);
            //     // servo_D2.setNormalisedPulseWidth(servo_D2_ang_max);
            //     servo_D0.setNormalisedPulseWidth(0.0f);
            //     servo_D1.setNormalisedPulseWidth(0.0f);
            //     servo_D2.setNormalisedPulseWidth(0.0f);
            // }

            // state machine
            switch (robot_state) {
            case RobotState::INIT:

                // // check if servos are enabled, should be alreay disabled at this
                // // point, it's just an example
                // if (!servo_D0.isEnabled())
                //     servo_D0.enable();
                // if (!servo_D1.isEnabled())
                //     servo_D1.enable();
                // if (!servo_D2.isEnabled())
                //     servo_D2.enable();

                enable_motors = 1; // enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled
                robot_state = RobotState::FORWARD;
                break;

            case RobotState::FORWARD:

                if (mechanical_button.read()) {
                    led2 = 1;

                    pwm_M1.write(0.75f); // write output voltage to motor M1
                    DCMotor_M2.setVelocity(0.5f); // set a desired speed for speed controlled dc motors M2
                    DCMotor_M3.setRotation(5.0f); // set a desired rotation for position controlled dc motors M3

                    robot_state = RobotState::BACKWARD;
                }
                break;

            case RobotState::BACKWARD:

                if (DCMotor_M3.getRotation() >= 4.95f) {
                    pwm_M1.write(0.25f);
                    DCMotor_M2.setVelocity(-0.5f);
                    DCMotor_M3.setRotation(0.0f);

                    robot_state = RobotState::SLEEP;
                }
                break;

            case RobotState::SLEEP:

                if (DCMotor_M3.getRotation() <= 0.05f) {
                    // enable_motors = 0;
                    pwm_M1.write(0.5f);
                    DCMotor_M2.setVelocity(0.0f);

                    // robot_stat is not changed, there for the state machine remains in
                    // here until the blue button is pressed again
                }

            default:

                // do nothing
                break;
            }
        } else {
            if (do_reset_all_once) {
                do_reset_all_once = false;

                ir_distance_mV = 0.0f;

                pwm_M1.write(0.5f);
                DCMotor_M2.setVelocity(0.0f);
                DCMotor_M3.setRotation(0.0f);
                robot_state = RobotState::INIT;
                servo_angle = 0.0f;
                // // servo_D0.setNormalisedPulseWidth(servo_angle);
                // // servo_D1.setNormalisedPulseWidth(servo_angle);
                // // servo_D2.setNormalisedPulseWidth(servo_angle);
                // // servo_D0.setNormalisedPulseWidth(servo_D0_ang_min);
                // // servo_D1.setNormalisedPulseWidth(servo_D1_ang_min);
                // // servo_D2.setNormalisedPulseWidth(servo_D2_ang_min);
                // servo_D0.setNormalisedPulseWidth(1.0f);
                // servo_D1.setNormalisedPulseWidth(1.0f);
                // servo_D2.setNormalisedPulseWidth(1.0f);
                // // Those commands under are causing servo to freeze and not go back to
                // // possision 0.0 servo_D0.disable(); servo_D1.disable();
                // // servo_D2.disable();
                // //  servo_D0.setNormalisedPulseWidth(pulse_min);
                // //  servo_D1.setNormalisedPulseWidth(pulse_min);

                led1 = led2 = 0;
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
        //        DCMotor_M3.getRotation(),
        //        servo_angle,
        //        servo_D1_angle);
        // printf("%f\n", servo_angle);
        int time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count();
        DCMotor* DCMotor_ptr = &DCMotor_M3;
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

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

void user_button_pressed_fcn()
{
    // do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    if (do_execute_main_task) {
        do_reset_all_once = true;
    }
}