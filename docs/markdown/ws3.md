# Workshop 3

<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/

In the third workshop, we will be using DC motors, exploring various control methods and understanding how to use the capabilities of the available drivers and hardware. DC motors are often used as drive systems, e.g. to drive a small vehicle or robot forward.

We will discuss different control strategies for motor control, e.g. speed controlled and position controller (angular speed and rotation). Additionally, we will revisit the construction of a state machine, incorporating sensor-equipped motors for distance measurement and integrating a mechanical button into the system.

## Hardware

>Part 1:
> - PES board with NUCLEO-F446RE board
> - Mini USB cable
> - DC motor
> - Jumper wires

>Part 2:
> - PES board with NUCLEO-F446RE board
> - Mini USB cable
> - Ultrasonic sensor 
> - Mechanical button
> - DC motor
> - Additional wires to connect the sensor to the NUCLEO board
> - Jumper wires

### Assignment

## Part 1

In the first task, we will focus solely on understanding the motor functionality and control mechanisms. To achieve this, we'll rely on the information provided in the manual, supplemented by guidance from the hardware tutorial.

1. First, familiarize yourself with the safety rules for using a DC motor. 
2. Read the instructions for DC motors to the point above the section [DC Motor Driver](../markdown/dc_motor.md#dc-motor-driver)
3. Connect the motor to the pin row **M1**, see [Connection to the PES-Board](../markdown/dc_motor.md#connection-to-the-pes-board)
4. Proceed to understand how to enable the power electronics, as outlined in the next step of the instructions.
5. Continue with the [Create DC motor Object and command the DC motor](../markdown/dc_motor.md#create-dc-motor-object-and-command-the-dc-motor) and proceed with the subsequent commands for motors M1, M2, and M3.<br> **Important note: In this tutorial, the motors are consistently connected to the same pin. However, it's worth noting that there is an option to connect them to different pins: M2 and M3. You can run up to 3 DC motors with one PES board.**

## Part 2

In the second part, we'll design a state machine using the hardware introduced in the previous workshop. The state machine will consist of five states:

**0. Initial** <br>
**1. Sleep** <br>
**2. Forward** <br>
**3. Backward** <br>
**4. Emergency** <br>

The ultimate objective is to construct a mechatronic system that mimics a can crusher press. Pressing the mechanical button will prompt to the **Forward** state in which the motor will move forward a specific number of revolutions (representing the press going down), and then, after reaching specyfic number of rotation, backward to its initial position and then to the **Sleep** state. If the distance from the ultrasonic sensor while being in **Forward** state is too small (e.g. an obstacle is in the way), the device should switch to emergency state, so rapidly return to its initial position and shut down.

Before doing the task you may look at the [Structuring a Robot Task Tutorial](../markdown/tips.md#structuring-a-robot-task).

1. Create flow chart according to the description above.
2. Connect the mechanical button to the PC_5 pin and the ground to the corresponding pin (see [Nucleo Board Pinmap][1])

3. In the ``main`` function, it's essential to create a mechanical button object with the appropriate pullup mode

```
// mechanical button
DigitalIn mechanical_button(PC_5); // create DigitalIn object to evaluate mechanical button, you
                                   // need to specify the mode for proper usage, see below
mechanical_button.mode(PullUp);    // sets pullup between pin and 3.3 V, so that there
                                   // is a defined potential
```

4. Connect the ultrasonic sensor to the D3 pins on the PES board (see [PES Board Pinmap](../datasheets/pes_board_peripherals.pdf)) 

5. In the ``main`` function, it's essential to include the apropriate driver and create the ultrasonic sensor object along with a variable to store the value. For more details refer to [Ultrasonic Distance Sernsor](../markdown/ultrasonic_sensor.md)

```
#include "pm2_drivers/UltrasonicSensor.h"

...

UltrasonicSensor us_sensor(PB_D3);
float us_distance_cm = 0.0f;
```

6. Within the while loop, include the following command to enable distance reading from the sensor regardless of the robot's current state.

```
us_distance_cm = us_sensor.read();
```

7. Make sure that you added statement to handle non-valid measurment below distance reading command e.g.:

```
if (us_distance_cm < 0.0f) {
    us_distance_cm = 0.0f;
}
```

8. Create an object for [Motor M3](../markdown/dc_motor.md#motor-m3), which will be controlled by setting its position. Activate the motion planner and configure the maximum acceleration to 0.5 of the value assigned to the motor by default.

```
const float voltage_max = 12.0f; // maximum voltage of battery packs, adjust this to 
                                 // 6.0f V if you only use one battery pack

// motor M3
const float gear_ratio_M3 = 78.125f;
const float kn_M3 = 180.0f / 12.0f;
DCMotor motor_M3(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio_M3, kn_M3, voltage_max);
motor_M3.setEnableMotionPlanner(true);
motor_M3.setMaxAcceleration(0.5 * motor_M3.getMaxAcceleration());
```

9. At the start of the ``main`` function, include the enumerators representing different robot states:

```
enum RobotState {
    INITIAL,
    SLEEP,      
    FORWARD,
    BACKWARD,
    EMERGENCY
} robot_state = RobotState::INITIAL;
```

10. Then in the infinite loop after triggering the statement (do execute main task), place a blank template of the state machine:

```
switch (robot_state) {
    case RobotState::INITIAL: {

        break;
    }
    case RobotState::SLEEP: {

        break;
    }
    case RobotState::FORWARD: {

        break;
    }
    case RobotState::BACKWARD: {

        break;
    }
    case RobotState::EMERGENCY: {

        break;
    }
    default:
        break;
}
```

11. Insert the DC motor enable statement in the initial state case. Subsequently, specify the next state, which is the **Sleep** state

```
case RobotState::INITIAL: {
    enable_motors = 1;
    robot_state = RobotState::SLEEP;

    break;
}
```

12. The sleep state is the state in which system will wait for the signal to execute task, so to go to forward case, this signal will be provided by clicking mechanical button.

```
case RobotState::SLEEP: {
    if (mechanical_button.read()) {
        robot_state = RobotState::FORWARD;
    }
    break;
}
```

13.  In the Forward case, include a command to execute 10 forward rotations and a condition that transitions to another state (**Backward**) after reaching 10 revolutions. Also, add a condition that, if the distance measured by the ultrasonic sensor is less than 7 cm, the system will enter the emergency state.

```
case RobotState::FORWARD: {
    motor_M3.setRotation(10.0f);
    if (motor_M3.getRotation() > 9.95f) {
        robot_state = RobotState::BACKWARD;
    }
    if (us_distance_cm < 7.0f) {
        robot_state = RobotState::EMERGENCY;
    }

    break;
}
```

14.  In the Backward case, the device returns to the initial position. Additionally, include a statement to transition to the Sleep state after reaching that position.

```
case RobotState::BACKWARD: {
    motor_M3.setRotation(0.0f);
    robot_state = RobotState::SLEEP;

    break;
}
```

15.  In the **Emergency** state, the machine needs to quickly return to the initial position and turn off. To achieve this, disable the motion planner, as it allows for the fastest possible movement. Subsequently, turn off the machine, simulating the effect of pressing the emergency button. To reuse the mechatronic system, reset the machine using the **RESET** button. Inside the **Emergency** state:

```
case RobotState::EMERGENCY: {
    motor_M3.setEnableMotionPlanner(false);
    motor_M3.setRotation(0.0f);
    if (motor_M3.getRotation() < 0.05f) {
        toggle_do_execute_main_fcn();
    }
    break;
}
```

16.  In the end, add a printing command to read the distance measured by the sensor and motor's rotations.

```
printf("%f, %f \n", us_distance_cm, motor_M3.getRotation());
```

17. Include the following command in the else statement, triggered by pressing the USER button while the program is running, to reset the variables to their initial values without restarting the program.

```
// reset variables and objects
led1 = 0;
enable_motors = 0;
us_distance_cm = 0.0f;
```

18.   Upload the program to the microcontroller using the **PLAY** button in Mbed Studio. Then, aim the sensor at an object that is beyond the distance triggering the emergency state. Press the **USER** button, and subsequently, click the mechanical button.
19.   Experiment by pressing the mechanical button and pointing the sensor at an object that is below the threshold specified in the code.
20.   After finishing unplug the Nucleo board.

## Summary

In the third workshop, exploration into the intricacies of DC motors was done involved understanding various control methods and utilizing the capabilities of the controller. The first segment focused on comprehending DC motors functionality and control mechanisms, while the subsequent part centered on refining a state machine. The incorporation of mechanical buttons, ultrasonic sensors, and the implementation of a state machine provided practical insights into the control of mechatronic systems.

Questions for own consideration:
- Why is it crucial that the program execution starts only after a specific action, such as pressing the **USER** button?

## Solutions

- [Workshop 3, Part 1](../solutions/main_ws3_p1.cpp)
- [Workshop 3, Part 2](../solutions/main_ws3_p2.cpp)

<p align="center">
    <img src="../images/ws3_flowchart.png" alt="Flow Chart for Workshop 3" width="500" /> </br>
    <i>Flow Chart for Workshop 3</i>
</p>