# Workshop 2

## Purpose of workshop 2
The objective of the second workshop is to enhance comprehension of the PES board's construction and functionality by incorporating additional hardware, specifically a servo. Furthermore, participants will be introduced to new tools, including the state machine, which is crucial for developing accurate software for the robot. 

## Hardware
>First part:
> - PES board with NUCLEO-F446RE board
> - Mini USB cable
> - Servo Futaba S3001/RELY S-009
> - Additional wires to connect the servo to the board
> - Mechanical button
> - Jumper wires

<!-- TODO fill it out -->
<!-- >Second part:
> - NUCLEO-F446RE board
> - Mini USB cable
> - IR sensor (check which one you have, the model name is on the side, it will determine the range of measurement)
> - Additional wires to connect the sensor to the NUCLEO board
> - Paper tape
> - Length measure tape -->

## Assignment
We assume that you know the general structure of the PES board, if not, go to: [PES board](../../README.md#pes-board)
### First part
------------------
The first task will be servo integration and calibration along with mechanical button integration.

1. Connect the mechanical button to the PC_5 pin and the ground to the corresponding pin (use [Nucleo Board pinmap][1])
2. After connecting mechanical button, connect the board to the computer using a mini USB cable. Immediately after connecting, a console window should appear in Mbed studio.
3. In ``main`` function you need to create mechanical button object, with proper pullup mode:
```
// mechanical button
// create DigitalIn object to evaluate extra mechanical button
DigitalIn mechanical_button(PC_5);
// set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential
mechanical_button.mode(PullUp);
```
4. Connect the servos to the D0 and D1 pins on the PES board (use [PES Board pinmap](../datasheets/pes_board_peripherals.pdf))
5. Read the Servo manual, the calibration process is described there step by step. Perform calibrations according to the instruction

    > [Servo tutorial](../markdown/servo.md)

5. Unplug Nucleo board from computer.
Remember: **If you have any questions, just ask them.**

<!-- TODO change it to US sensor -->
### Second part
------------------
The second task will be to design a state machine, which will use additional hardware, which was the subject of the previous workshop, i. e. IR distance sensor.In this task we will create a state machine, which will have 4 states:
- initial
- execution
- sleep
- emergency

The general assumption is to construct a mechatronic system that will adjust the deflection of the servo lever depending on the distance measured by the sensor. Sleep state will be the state to which the system will go after pressing the mechanical button, and the system will go into emergency state if the distance measured by the sensor is too small.

Before doing task look at the .......

1. Connect the IR distance sensor to the PC_2 pin and the ground to the corresponding pin (use [Nucleo Board pinmap][1])
2. In ``main`` function at the beginning create robot states:
```
enum RobotState {
    INITIAL,      
    EXECUTION,
    SLEEP,
    EMERGENCY
} robot_state = RobotState::INITIAL;
```
3. Then in the infinite loop after triggering statement (do execute main task), place blank template of state machnie:
```
switch (robot_state) {
    case RobotState::INITIAL: {

        break;
    }
    case RobotState::EXECUTION: {

        break;
    }
    case RobotState::SLEEP: {

        break;
    }
    case RobotState::EMERGENCY: {

        break;
    }
    default: {
        break; 
    }
}
```
4. Move enabling statements to the initial state case. Add function specifying new robot state - execution.
```
case RobotState::INITIAL:
    if (!servo_D0.isEnabled())
        servo_D0.enable();
    robot_state = RobotState::EXECUTION;
    break;
```
5. In the next step you need to enter a routine that will convert the measured distance to the tilting of the servo mechanism. As we know servos are calibrated so our goal is to map servo mechanisms so that the minimum sensor range is zero servo deflection and the maximum range is the maximum servo deflection. Below the sensor definition enter the minimum and maximum range.
```
float min_range = 4.0f;
float max_range = 40.0f;
```
6. Now you need to enter function that will map distance measurments to servo deflation along with servo commanding statement:
```
case RobotState::EXECUTION: {
    //Function to map the distnace to servo movement
    float servo_ang = (1 / (max_range - min_range)) * ir_distance_cm - (min_range / (max_range - min_range));
    servo_D0.setNormalisedPulseWidth(servo_ang);

    break;
}
```
7. Now it is time for conditions that will make us jump to another states. As we mentioned sleep state will be initialized when pressing mechanical button and emergency state will be initialized when range will be too small. Inside the execution state:
```
case RobotState::EXECUTION: {
    //Function to map the distnace to servo movement
    float servo_ang = (1 / (max_range - min_range)) * ir_distance_cm - (min_range / (max_range - min_range));
    servo_D0.setNormalisedPulseWidth(servo_ang);

    if (mechanical_button.read()){
        robot_state = RobotState::SLEEP;
    }
    else if (ir_distance_cm > max_range) {
        robot_state = RobotState::EMERGENCY;
    }

    break;
}
```
8. To jump back from sleep to execution we also need to click the button. Inside sleep state:
```
case RobotState::SLEEP: {
    if (mechanical_button.read()){
        robot_state = RobotState::EXECUTION;
    }
    break;
}
```
9. Emergency state is the state when are values are set to 0 and servos are disabled. Inside emergency state:
```
case RobotState::EMERGENCY: {
    user_button_pressed_fcn();  
    break;
}
```
10. To use the mechatronic system once again you need to reset the machine with **RESET** button.

## Summary


<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/
