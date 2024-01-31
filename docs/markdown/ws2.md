# Workshop 2

## Scope of Workshop 2

The objective of the second workshop is to enhance comprehension of the PES board's hardware and functionality by incorporating additional sensors and actuators, specifically a servo. Furthermore, participants will be introduced to new tools/programming concepts, including a state machine, which is crucial for developing reliable software for the robot. 

## Hardware

>Part 1:
> - PES board with NUCLEO-F446RE board
> - Mini USB cable
> - Servo Futaba S3001/RELY S-0090
> - Additional wires to connect the servo to the board
> - Mechanical button
> - Jumper wires

>Part 2:
> - PES board with NUCLEO-F446RE board
> - Mini USB cable
> - Ultrasonic sensor 
> - Mechanical button
> - Servo Futaba S3001/REELY S-0090
> - Additional wires to connect the sensor to the NUCLEO board
> - Jumper wires

## Assignment

We assume that you know the general structure of the PES board, if not, please visit: [PES board](../../README.md#pes-board)

### Part 1
------------------
<!-- TODO: Do we want them to perform this for one or two servo motors?
           We should check if the calibration of one servo can be used at an other servo
           of the same type as well? -->
The first task will be the integration of two servo motors and their calibration along with the usage of a mechanical button.

1. Connect the mechanical button to the PC_5 pin and the ground to the corresponding pin (see [Nucleo Board pinmap][1])
2. In the ``main`` function, it's essential to create a mechanical button object with the appropriate pullup mode:

```
// mechanical button
DigitalIn mechanical_button(PC_5); // create DigitalIn object to evaluate mechanical button, you
                                   // need to specify the mode for proper usage, see below
mechanical_button.mode(PullUp);    // sets pullup between pin and 3.3 V, so that there
                                   // is a defined potential
```

3. Connect the servos to the D0 and D1 pins on the PES board (see [PES Board pinmap](../datasheets/pes_board_peripherals.pdf))
4. Refer to the Servo manual for a step-by-step guide on the calibration process. Follow the instructions outlined in the manual to complete the calibration.

    > [Servo tutorial](../markdown/servo.md)

<!-- TODO: Describe the purpose of the mechanical button and how it should be integrated here,
           if it is not used in part 1, then move it to part 2 -->

5. Unplug the Nucleo board.
<br>

<!-- TODO: Why do we want the students to unplug the nucleo board here? This comments hold for all workshops. -->

### Part 2
------------------
The second task will be to design a state machine, where we will use additionaly an ultrasonic sensor. Here a state machine will be created, with the four following states:

0. **Initial**
1. **Execution**
2. **Sleep**
3. **Emergency**

The overall goal is to build a mechatronic system capable of adjusting the deflection of the servo lever based on the distance measured by the sensor. The system will enter a sleep state if readings are out of range, and pressing the mechanical button will trigger an emergency state/stop.

Before doing the task you may look at [structuring a robot task tutorial](../markdown/tips.md#structuring-a-robot-task).

1. Connect the ultrasonic sensor to the D3 pins on the PES board (see [PES Board pinmap](../datasheets/pes_board_peripherals.pdf))
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

<!-- TODO: Would be nice if we had a flowchart diagram according to the statemachine -->

1. Then in the while loop after triggering the statement (do execute main task), place the blank template of state machnie:

```
switch (robot_state) {
    case RobotState::INITIAL:

        break;
    case RobotState::EXECUTION:

        break;
    case RobotState::SLEEP:

        break;
    case RobotState::EMERGENCY:

        break;
    default:
        break;
}
```

1. Move the enabling statements to the initial state case. Add function specifying new robot states - execution.

```
case RobotState::INITIAL:
    // enable the servo
    if (!servo_D0.isEnabled())
        servo_D0.enable();
    robot_state = RobotState::EXECUTION;
    break;
```
<!-- TODO: This needs to be written clearer please. -->
2. In the next step you need to enter a routine that will convert the measured distance to the tilting of the servo mechanism. As we know servos are calibrated so our goal is to map the servo so that the minimum sensor range is zero servo deflection and the maximum range is the maximum servo deflection. Below the sensor definition enter the minimum and maximum range, you can use a smaller range included in the measurement range:

```
float us_distance_min = 4.0f;
float us_distance_max = 50.0f;
```

3. Enter the function that will map the distance measurments to servo the deflation along with the servo commanding statement:

```
case RobotState::EXECUTION:
    // function to map the distance to the servo movement
    float servo_input = (1 / (us_distance_max - us_distance_min)) * us_distance_cm - (us_distance_min / (us_distance_max - us_distance_min));
    servo_D0.setNormalisedPulseWidth(servo_input);

    break;
```

4. Now, let's establish the conditions that prompt transitions to other states. As previously mentioned, pressing the mechanical button will trigger the initiation of the sleep state, while the emergency state will be initialized when the sensor readout falls below the minimum range. Within the execution state:

```
case RobotState::EXECUTION:
    // function to map the distance to the servo movement
    float servo_input = (1 / (us_distance_max - us_distance_min)) * us_distance_cm - (us_distance_min / (us_distance_max - us_distance_min));
    servo_D0.setNormalisedPulseWidth(servo_input);

    // if the measurement is outside the min and max range go to SLEEP
    if ((us_distance_cm < us_distance_min) || (us_distance_cm > us_distance_max)) {
        robot_state = RobotState::SLEEP;
    // TODO: Don't you want this statement in a seperate if (independently)?
    } else if (mechanical_button.read()) {
        robot_state = RobotState::EMERGENCY;
    }

    break;
```

5. The serial terminal window will display the message "SLEEP MODE". To transition back from the sleep state to execution, the sensor readings must fall within the specified range. Further more, the emergency switch can also be activated in the sleep state. Within the sleep state place:

```
case RobotState::SLEEP:
    // TODO: Why do we print to the terminal here?
    printf("SLEEP MODE \n");

    // if the measurement is within the min and max range go to EXECUTION
    if ((us_distance_cm > us_distance_min) || (us_distance_cm < us_distance_max)) {
        robot_state = RobotState::EXECUTION;
    // TODO: Don't you want this statement in a seperate if (independently)?
    } else if (mechanical_button.read()) {
        robot_state = RobotState::EMERGENCY;
    }

    break;
```
<!-- TODO: Please rewrite this. -->
6. The emergency state is the state when are values are set to 0 and servos are disabled. To use the mechatronic system once again you need to reset the machine with **RESET** button. Inside emergency state:

```
case RobotState::EMERGENCY:
    // TODO: We should create a new function that calls this function
    //       Please insert some comments here to describe what is happening
    user_button_pressed_fcn();

    break;
```

7.  Upload the program to the microcontroller using the **PLAY** button in Mbed studio and then point the sensor at an object that is at a distance that is within the range specified in the code and click the **USER** button.

8.  Experiment by directing the sensor towards an object that is out of range. Press the mechanical button and observe the serial terminal for the outcome.

9.  After finishing unplug Nucleo board from computer.

## Summary

In the second workshop, the integration of a servo along with the PES board, servo calibration, and mechanical button incorporation were emphasized. Additionally, the creation of a state machine using an ultrasonic sensor was explored. By establishing robot states and implementing transition conditions, a mechatronic system capable of adjusting a servo lever based on sensor readings was achieved. This workshop delivered hands-on experience in hardware integration and state machine design.

Questions for own consideration:
<!-- TODO think about it, about those question -->
- THINK ABOUT IT

## Solution

[Workshop 2, Part 1](../solutions/main_ws2_p1.txt)

[Workshop 2, Part 2](../solutions/main_ws2_p2.txt)

<!-- TODO: Place grafic of the evaluation here -->

<!-- TODO: Place grafic of flow-chart diagram here -->

<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/
