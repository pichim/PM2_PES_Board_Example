#include <mbed.h>

#include "PM2_Drivers.h"

# define M_PI 3.14159265358979323846  // number pi

// logical variable main task
bool do_execute_main_task = false;  // this variable will be toggled via the user button (blue button) to or not to execute the main task

// user button on nucleo board
Timer user_button_timer;            // create Timer object which we use to check if user button was pressed for a certain time (robust against signal bouncing)
InterruptIn user_button(PC_13);     // create InterruptIn interface object to evaluate user button falling and rising edge (no blocking code in ISR)
void user_button_pressed_fcn();     // custom functions which gets executed when user button gets pressed and released, definition below
void user_button_released_fcn();

int main()
{
    // while loop gets executed every main_task_period_ms milliseconds
    const int main_task_period_ms = 50;   // define main task period time in ms e.g. 50 ms -> main task runns 20 times per second
    Timer main_task_timer;                // create Timer object which we use to run the main task every main task period time in ms

    // led on nucleo board
    DigitalOut user_led(LED1);      // create DigitalOut object to command user led

    // additional Led
    DigitalOut extra_led(PB_9);     // create DigitalOut object to command extra led (do add an aditional resistor, e.g. 220...500 Ohm)

    // mechanical button
    DigitalIn mechanical_button(PC_5);  // create DigitalIn object to evaluate extra mechanical button, you need to specify the mode for proper usage, see below

    // Sharp GP2Y0A41SK0F, 4-40 cm IR Sensor
    float ir_distance_mV = 0.0f;    // define variable to store measurement
    AnalogIn ir_analog_in(PC_2);    // create AnalogIn object to read in infrared distance sensor, 0...3.3V are mapped to 0...1

    // 78:1, 100:1, ... Metal Gearmotor 20Dx44L mm 12V CB
    DigitalOut enable_motors(PB_15);    // create DigitalOut object to enable dc motors

    const float pwm_period_s = 0.00005f; // define pwm period time in seconds and create FastPWM objects to command dc motor M1
    FastPWM pwm_M1(PB_13);               // motor M1 is used open loop
    FastPWM pwm_M2(PA_9);                // motor M2 is closed-loop speed controlled (angle velocity)
    FastPWM pwm_M3(PA_10);               // motor M3 is closed-loop position controlled (angle controlled)

    EncoderCounter  encoder_M1(PA_6, PC_7); // create encoder objects to read in the encoder counter values
    EncoderCounter  encoder_M2(PB_6, PB_7);
    EncoderCounter  encoder_M3(PA_0, PA_1);

    // create SpeedController and PositionController objects, default parametrization is for 78.125:1 gear box
    const float max_voltage = 12.0f;                  // define maximum voltage of battery packs, adjust this to 6.0f V if you only use one batterypack
    const float counts_per_turn = 20.0f * 78.125f;    // define counts per turn at gearbox end: counts/turn * gearratio
    const float kn = 180.0f / 12.0f;                  // define motor constant in rpm per V
    const float k_gear = 100.0f / 78.125f;            // define additional ratio in case you are using a dc motor with a different gear box, e.g. 100:1
    const float kp = 0.1f;                            // define custom kp, this is the default speed controller gain for gear box 78.125:1

    // SpeedController speedController_M2(counts_per_turn, kn, max_voltage, pwm_M2, encoder_M2); // default 78.125:1 gear box  with default contoller parameters
    SpeedController speedController_M2(counts_per_turn * k_gear, kn / k_gear, max_voltage, pwm_M2, encoder_M2); // parameters adjusted to 100:1 gear

    float max_speed_rps = 0.5f;                 // define maximum speed that the position controller is changig the speed, has to be smaller or equal to kn * max_voltage
    // PositionController positionController_M3(counts_per_turn, kn, max_voltage, pwm_M3, encoder_M3); // default 78.125:1 gear with default contoller parameters
    PositionController positionController_M3(counts_per_turn * k_gear, kn / k_gear, max_voltage, pwm_M3, encoder_M3); // parameters adjusted to 100:1 gear, we need a different speed controller gain here
    positionController_M3.setSpeedCntrlGain(kp * k_gear);
    positionController_M3.setMaxVelocityRPS(max_speed_rps);

    // Futaba Servo S3001 20mm 3kg Analog
    Servo servo_S1(PB_2);           // create servo objects
    Servo servo_S2(PC_8);
    float servo_S1_angle = 0;       // servo S1 normalized angle
    float servo_S2_angle = 0;       // servo S2 normalized angle
    const int servo_period_mus = 20000;   // define servo period time in mus

    int servo_counter = 0;          // define servo counter, this is an additional variable to make the servos move
    const int loops_per_seconds = static_cast<int>(ceilf(1.0f/(0.001f*(float)main_task_period_ms))); // define loops per second

    // Groove Ultrasonic Ranger V2.0
    float us_distance_cm = 0.0f;    // define variable to store measurement
    RangeFinder us_range_finder(PB_12, 5782.0f, 0.02f, 17500); // create range finder object (ultra sonic distance sensor), 20 Hz parametrization
    // RangeFinder us_range_finder(PB_12, 5782.0f, 0.02f,  7000); // create range finder object (ultra sonic distance sensor), 50 Hz parametrization

    // LSM9DS1 IMU, carefull: not all PES boards have an imu (chip shortage)
    // LSM9DS1 imu(PC_9, PA_8); // create LSM9DS1 comunication object, if you want to be able to use the imu you need to #include "LSM9DS1_i2c.h"

    // attach button fall and rise functions to user button object
    user_button.fall(&user_button_pressed_fcn);
    user_button.rise(&user_button_released_fcn);

    // start timer
    main_task_timer.start();

    // set pullup mode: add resistor between pin and 3.3 V, so that there is a defined potential
    mechanical_button.mode(PullUp);

    // enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled
    enable_motors = 1;

    // motor M1 is used open-loop, we need to initialize the pwm and set pwm output to zero at the beginning, range: 0...1 -> u_min...u_max: 0.5 -> 0 V
    pwm_M1.period(pwm_period_s);
    pwm_M1.write(0.5f);

    // set the soft pwm period for the servo objects
    servo_S1.SetPeriod(servo_period_mus);
    servo_S2.SetPeriod(servo_period_mus);

    while (true) { // this loop will run forever

        main_task_timer.reset();

        if (do_execute_main_task) {

            // read analog input
            ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;

            // command dc motors if mechanical button is pressed
            if (mechanical_button.read()) {
                pwm_M1.write(0.75f); // write output voltage to motor M1
                speedController_M2.setDesiredSpeedRPS(0.5f); // set a desired speed for speed controlled dc motors M2
                positionController_M3.setDesiredRotation(1.5f); // set a desired rotation for position controlled dc motors M3
            } else {
                pwm_M1.write(0.5f);
                speedController_M2.setDesiredSpeedRPS(0.0f);
                positionController_M3.setDesiredRotation(0.0f);
            }

            // check if servos are enabled
            if (!servo_S1.isEnabled()) servo_S1.Enable();
            if (!servo_S2.isEnabled()) servo_S2.Enable();
            // command servo position, this needs to be calibrated
            servo_S1.SetPosition(servo_S1_angle);
            if (servo_S1_angle < 1.0f & servo_counter%loops_per_seconds == 0 & servo_counter != 0) {
                servo_S1_angle += 0.01f;
            }
            servo_S2.SetPosition(servo_S2_angle);
            if (servo_S2_angle < 1.0f & servo_counter%loops_per_seconds == 0 & servo_counter != 0) {
                servo_S2_angle += 0.01f;
            }
            servo_counter++;

            // read ultra sonic distance sensor
            us_distance_cm = us_range_finder.read_cm();

            // visual feedback that the main task is executed
            extra_led = 1;

        } else {

            ir_distance_mV = 0.0f;

            pwm_M1.write(0.5f);
            speedController_M2.setDesiredSpeedRPS(0.0f);
            positionController_M3.setDesiredRotation(0.0f);

            servo_S1_angle = 0;
            servo_S2_angle = 0;
            // servo_S1.SetPosition(servo_S1_angle);
            // servo_S2.SetPosition(servo_S2_angle);
            if (servo_S1.isEnabled()) servo_S1.Disable();
            if (servo_S2.isEnabled()) servo_S2.Disable();

            us_distance_cm = 0.0f;

            extra_led = 0;
        }

        user_led = !user_led;

        // do only output via serial what's really necessary (this makes your code slow)
        printf("IR sensor (mV): %3.3f, Encoder M1: %3d, Speed M2 (rps) %3.3f, Position M3 (rot): %3.3f, Servo S1 angle (normalized): %3.3f, Servo S2 angle (normalized): %3.3f, US sensor (cm): %3.3f\r\n",
               ir_distance_mV,
               encoder_M1.read(),
               speedController_M2.getSpeedRPS(),
               positionController_M3.getRotation(),
               servo_S1_angle,
               servo_S2_angle,
               us_distance_cm);

        // read out the imu, the actual frames of the sensor reading needs to be figured out
        // imu.updateGyro();
        // imu.updateAcc();
        // imu.updateMag();
        // printf("%.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f, %.6f\r\n", imu.readGyroX(), imu.readGyroY(), imu.readGyroZ(),
        // imu.readAccX(), imu.readAccY(), imu.readAccZ(), imu.readMagX(), imu.readMagY(), imu.readMagZ());

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

void user_button_pressed_fcn()
{
    user_button_timer.start();
    user_button_timer.reset();
}

void user_button_released_fcn()
{
    // read timer and toggle do_execute_main_task if the button was pressed longer than the below specified time
    int user_button_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(user_button_timer.elapsed_time()).count();
    user_button_timer.stop();
    if (user_button_elapsed_time_ms > 200) {
        do_execute_main_task = !do_execute_main_task;
    }
}