#include "mbed.h"

#include "PESBoardPinMap.h"
#include "DebounceIn.h"
#include "DCMotor.h"
#include "SensorBar.h"
#include "ColorSensor.h"
#include "IMU.h"
#include "Servo.h"

#include "GimbalController.h"
#include "MissionController.h"
#include "RobotConfig.h"
#include "RobotHardware.h"
#include "RobotTypes.h"

namespace {
bool do_execute_main_task = false;
bool do_reset_all_once = false;

DebounceIn user_button(BUTTON1);

void toggle_do_execute_main_fcn()
{
    do_execute_main_task = !do_execute_main_task;
    if (do_execute_main_task) {
        do_reset_all_once = true;
    }
}
}

int main()
{
    user_button.fall(&toggle_do_execute_main_fcn);

    DigitalOut user_led(LED1);
    DigitalOut enable_motors(PB_ENABLE_DCMOTORS);

    DCMotor motor_M1(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1,
                     RobotConfig::GEAR_RATIO, RobotConfig::SPEED_PER_VOLT,
                     RobotConfig::VOLTAGE_MAX);
    DCMotor motor_M2(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2,
                     RobotConfig::GEAR_RATIO, RobotConfig::SPEED_PER_VOLT,
                     RobotConfig::VOLTAGE_MAX);
    DCMotor motor_M3(PB_PWM_M3, PB_ENC_A_M3, PB_ENC_B_M3,
                     RobotConfig::RACK_GEAR_RATIO, RobotConfig::RACK_SPEED_PER_VOLT,
                     RobotConfig::VOLTAGE_MAX);

    motor_M1.enableMotionPlanner();
    motor_M2.enableMotionPlanner();
    motor_M3.enableMotionPlanner();
    motor_M1.setMaxAcceleration(motor_M1.getMaxAcceleration() * 0.8f);
    motor_M2.setMaxAcceleration(motor_M2.getMaxAcceleration() * 0.8f);
    motor_M3.setMaxAcceleration(motor_M3.getMaxAcceleration() * 0.8f);

    SensorBar sensor_bar(PB_9, PB_8, RobotConfig::SENSOR_BAR_DISTANCE_M, true);
    ColorSensor colour_sensor(PB_3);
    colour_sensor.switchLed(ON);

    IMU imu(PC_9, PA_8);
    Servo servo_roll(PB_D0);
    Servo servo_pitch(PB_D1);

    RobotHardware hardware{
        motor_M1, motor_M2, motor_M3,
        sensor_bar, colour_sensor, imu,
        servo_roll, servo_pitch, enable_motors
    };
    GimbalController gimbal(servo_roll, servo_pitch);
    MissionController mission;
    Timer main_task_timer;
    main_task_timer.start();

    while (true) {
        main_task_timer.reset();

        SensorReadings readings;
        readings.colour = colour_sensor.getColor();
        readings.line_visible = sensor_bar.isAnyLedActive();
        if (readings.line_visible) {
            readings.line_angle_rad = sensor_bar.getAvgAngleRad();
        }

        const ImuData imu_data = imu.getImuData();
        readings.roll_rad = imu_data.pry(1);
        readings.pitch_rad = imu_data.pry(0);

        if (do_execute_main_task) {
            gimbal.update(readings.roll_rad, readings.pitch_rad);
            mission.update(hardware, readings);
            if (mission.isFinished()) {
                toggle_do_execute_main_fcn();
            }
        } else if (do_reset_all_once) {
            do_reset_all_once = false;
            mission.reset();
            gimbal.reset();
            motor_M1.setVelocity(0.0f);
            motor_M2.setVelocity(0.0f);
            motor_M3.setRotation(0.0f);
        }

        user_led = !user_led;

        const int elapsed_ms = duration_cast<milliseconds>(main_task_timer.elapsed_time()).count();
        if (elapsed_ms > RobotConfig::MISSION_PERIOD_MS) {
            printf("Warning: Main task took longer than mission period\n");
        } else {
            thread_sleep_for(RobotConfig::MISSION_PERIOD_MS - elapsed_ms);
        }
    }
}
