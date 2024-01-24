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

>Second part:
> - PES board with NUCLEO-F446RE board
> - Mini USB cable
> - Ultrasonic sensor 
> - Mechanical button
> - Servo Futaba S3001/REELY S-009
> - Additional wires to connect the sensor to the NUCLEO board
> - Jumper wires

## Assignment
We assume that you know the general structure of the PES board, if not, go to: [PES board](../../README.md#pes-board)
### First part
------------------
The first task will be servo integration and calibration along with mechanical button integration.

1. Connect the mechanical button to the PC_5 pin and the ground to the corresponding pin (use [Nucleo Board pinmap][1])
2. In the ``main`` function, it's essential to create a mechanical button object with the appropriate pullup mode.:
```
// mechanical button
// create DigitalIn object to evaluate extra mechanical button
DigitalIn mechanical_button(PC_5);
// set pullup mode: sets pullup between pin and 3.3 V, so that there is a defined potential
mechanical_button.mode(PullUp);
```
3. Connect the servos to the D0 and D1 pins on the PES board (use [PES Board pinmap](../datasheets/pes_board_peripherals.pdf))
4. Refer to the Servo manual for a step-by-step guide on the calibration process. Follow the instructions outlined in the manual to complete the calibration.

    > [Servo tutorial](../markdown/servo.md)

5. After finishing unplug Nucleo board from computer.
<br>

Remember: **If you have any questions, just ask them.**

### Second part
------------------
The second task will be to design a state machine, which will use additional hardware - ultrasonic sensor.In this a state machine will be created, with 4 states:
- initial
- execution
- sleep
- emergency

The overall goal is to build a mechatronic system capable of adjusting the deflection of the servo lever based on the distance measured by the sensor. The system will enter a sleep state if readings are out of range, and pressing the mechanical button will trigger an emergency state.

Before doing task look at the 
<!-- TODO LINK TO STATE MACHINE TUTORIAL -->

1. Connect the ultrasonic sensor to the D3 pins on the PES board (use [PES Board pinmap](../datasheets/pes_board_peripherals.pdf))
2. Read the US sensor manual and create object in the ``main`` function
    >[US sensor](../markdown/ultrasonic_sensor.md)
3. At the beginning  ``main`` function create robot states:
```
enum RobotState {
    INITIAL,      
    EXECUTION,
    SLEEP,
    EMERGENCY
} robot_state = RobotState::INITIAL;
```
4. Then in the infinite loop after triggering statement (do execute main task), place blank template of state machnie:
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
5. Move enabling statements to the initial state case. Add function specifying new robot state - execution.
```
case RobotState::INITIAL:
    if (!servo_D0.isEnabled())
        servo_D0.enable();
    robot_state = RobotState::EXECUTION;
    break;
```
6. In the next step you need to enter a routine that will convert the measured distance to the tilting of the servo mechanism. As we know servos are calibrated so our goal is to map servo mechanisms so that the minimum sensor range is zero servo deflection and the maximum range is the maximum servo deflection. Below the sensor definition enter the minimum and maximum range, you can use a smaller range included in the measurement range:
```
float min_range = 5.0f;
float max_range = 50.0f;
```
7. Enter function that will map distance measurments to servo deflation along with servo commanding statement:
```
case RobotState::EXECUTION: {
    //Function to map the distnace to servo movement
    float servo_ang = (1 / (max_range - min_range)) * ir_distance_cm - (min_range / (max_range - min_range));
    servo_D0.setNormalisedPulseWidth(servo_ang);

    break;
}
```
8. Now, let's establish the conditions that prompt transitions to other states. As previously mentioned, pressing the mechanical button will trigger the initiation of the sleep state, while the emergency state will be initialized when the sensor readout falls below the minimum range. Within the execution state:
```
case RobotState::EXECUTION: {
    //Function to map the distnace to servo movement
    float servo_ang = (1 / (max_range - min_range)) * us_distance_cm - (min_range / (max_range - min_range));
    servo_D0.setNormalisedPulseWidth(servo_ang);

    if (us_distance_cm < min_range | us_distance_cm > max_range) {
        robot_state = RobotState::SLEEP;
    } else if (mechanical_button.read()) {
        robot_state = RobotState::EMERGENCY;
    }

    break;
}
```
9. The console window will display the message "SLEEP MODE". To transition back from the sleep state to execution, the sensor readings must fall within the specified range. What's more, the emergency switch can also be activated in the sleep state. Within the sleep state:
```
case RobotState::SLEEP: {
    printf("SLEEP MODE");
    if (us_distance_cm > min_range | us_distance_cm < max_range){
        robot_state = RobotState::EXECUTION;
    } else if (mechanical_button.read()) {
        robot_state = RobotState::EMERGENCY;
    }
    break;
}
```
10. Emergency state is the state when are values are set to 0 and servos are disabled. To use the mechatronic system once again you need to reset the machine with **RESET** button. Inside emergency state:
```
case RobotState::EMERGENCY: {
    user_button_pressed_fcn();  
    break;
}
```
11. Upload the program to the microcontroller using the **PLAY** button in Mbed studio and then point the sensor at an object that is at a distance that is within the range specified in the code and click **USER** button.
12. Experiment by directing the sensor towards an object that is out of range. Press the mechanical button and observe the console window for the outcome.
13. After finishing unplug Nucleo board from computer.

## Summary
In second workshop, the integration of a servo into the PES board, calibration learning, and mechanical button incorporation were emphasized. Additionally, the creation of a state machine using an ultrasonic sensor was explored. By establishing robot states and implementing transition conditions, a simulated mechatronic system capable of adjusting a servo lever based on sensor readings was achieved. This workshop delivered hands-on experience in hardware integration and state machine design, fostering proficiency in robotics and mechatronics.<br>
Questions for own consideration:
<!-- TODO think about it, about those question -->
- THINK ABOUT IT

**Answer:**
[WS2](../solutions/main_ws2.txt)

<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/
