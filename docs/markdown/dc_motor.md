# DC Motor

A direct current (DC) motor is a type of electric machine that converts electrical energy into mechanical energy. DC motors take electrical power through direct current, and convert this energy into mechanical rotation transmisioned by the electric field. DC motors use magnetic fields that occur from the electrical currents generated, which powers the movement of a rotor fixed within the output shaft. The output torque and speed depends upon both the electrical input and the design of the motor.

<center><img src="../images/dc_motor.png" alt="dc motor" width="300"/></center>
<center> <i>Example of dc motor with encoder</i> </center>

## Technical Specifications

| | 31:1 Metal Gearmotor 20Dx41L mm 12V CB | 78:1 Metal Gearmotor 20Dx43L mm 12V CB |488:1 Metal Gearmotor 20Dx46L mm 12V CB|
|-|-|-|-|
|**Dimensions**|
|Size                     | 20D x 43.2L mm | 20D x 44.7L mm |20D × 47.7L mm|
|Weight                   | 44 g           | 45 g           | 47 g         |
|Shaft diameter           | 4 mm           | 4 mm           | 4 mm         |
|**General specifications**|
|Gear ratio               | 31.25:1        | 78.125:1       | 488.28125:1  |
|No-load speed @ 12V      | 450 rpm        | 180 rpm        | 28 rpm       |
|Stall torque @ 12V       | 0.24 Nm        | 0.53 Nm        | 2.45 Nm      |
|Max output power @ 12V   | 2.8 W          | 2.5 W          | 1.5 W        |
|No-load speed @ 6V       | 225 rpm        | 90 rpm         | 14 rpm       |
|Stall torque @ 6V        | 0.12 Nm        | 0.26 Nm        | 1.27 Nm      |
|**Encoder**|
|Resolution               | 20 (5)         | 20 (5)         | 20 (5)       |

## Links

[31:1 Metal Gearmotor 20Dx41L mm 12V CB][1] <br>
[78:1 Metal Gearmotor 20Dx43L mm 12V CB][2] <br>
[488:1 Metal Gearmotor 20Dx46L mm 12V CB][3] <br>

<!-- link list, last updated 15.01.2023 -->
[1]: https://www.pololu.com/product/3475/specs
[2]: https://www.pololu.com/product/3477/specs
[3]: https://www.pololu.com/product/3485/specs
[4]: https://nathandumont.com/blog/h-bridge-tutorial
[5]: https://www.electronics-tutorials.ws/blog/pulse-width-modulation.html

## Datasheets including measrued static Characteristics

[20D Pololu Motors](../datasheets/pololu-20d-metal-gearmotors.pdf)

## Static Characteristics of the 78:1 Metal Gearmotor 20Dx43L mm 12V CB

<center><img src="../images/dc_motor_static_characteristics.png" alt="Static Characteristics of 78: DC Motor" width="650"/></center>
<center> <i>Approximate static characteristics of 78:1 DC Motor</i> </center>
<br>


- The above motor characteristics are conservative approximations based on a simplified model. Measured and precise motor characteristics ca be found in the datasheet [20D Pololu Motors](../datasheets/pololu-20d-metal-gearmotors.pdf). It is advisable to make judicious choices in selecting motors, incorporating safety margins to account for these inaccuracies and avoid operating the motor at its extreme parameters. The top-left graph demonstrates the correlation between increasing torque and amperage, while the graph on the right illustrates the relationship between increasing speed and voltage. The subsequent graphs integrate these characteristics, revealing the interplay between torque and speed, aiding in motor selection. To evaluate the appropriate gear ratio, it is necessary to consider the needed speed and torque values.
- The selection of different gear ratios significantly influences the capabilities of the drive system. While the motor itself remains the same, adjustments in gear ratios impact torque and speed values at the gear end. Opting for a lower gear ratio like 31:1 yields higher speeds at the expense of reduced available torque. Conversely, a higher gear ratio, such as 488:1, results in a comparatively lower maximum speed but offers significantly higher torque.
- During the design process, leveraging the presented characteristics proves beneficial for the appropriate motor selection. When aiming for high speeds, emphasizing the speed-voltage relationship is crucial, while lifting heavy objects places greater importance on torque characteristics.

## Encoder and Relative Positioning

- A magnetic encoder is a sensor device that utilizes magnets and sensors to measure the angle. It typically consists of a magnetically encoded disk attached to the rotating part and a sensor that detects changes in the magnetic field, converting them into electrical signals (pulses). This signal provides information about the rotational position of the object.
- It's very important to understand that the used magnetic encoders provide relative position information. This means that the encoder only returns counts about changes in position relative to the point where it was initialized/started counting, and the initial position upon power-up is considered as the zero reference point. Therefore and only if absolute measruments are needed a homing procedure needs to be done after every startup uf the system. The accuracy of the measurements relies on the consistency of this referencing. Any absolute positioning requires additional sensors and additional code to perform the homing procedure.
<!-- - The encoder resolution significantly impacts the performance of a PID controller for DC motors. A higher encoder resolution provides more detailed feedback about the motor's position, enabling the PID controller to make finer adjustments and achieve greater precision in controlling the motor's speed and position. This increased resolution allows the PID controller to respond more accurately to deviations from the desired setpoint, resulting in smoother and more controlled motor movements. Conversely, a lower encoder resolution may limit the controller's ability to detect small changes, potentially leading to less accurate control and decreased overall performance. -->

## Practical Tips

- It's crucial to note that any closed-loop control system can only work properly when the encoder readings align with the motor's rotation direction. To ensure proper operation, establish the positive direction first, then connect the components to ensure both the rotation direction and encoder readings are positive. To alter the motor's movement direction, adjust the power supply connection to the M1 and M2 outputs accordingly, and do the same for the encoder using the A and B outputs. 

## DC Motor Driver

The ``DCMotor`` class is a versatile tool designed for controlling the velocity and/or rotation of a DC motor with precision. It incorporates essential components like an EncoderCounter, FastPWM, Motion control, PID controller, and an IIR Filter to ensure accurate control. This class provides user-friendly methods for adjusting velocity, rotation, control gains, and obtaining the current state of the motor, offering a comprehensive set of functionalities for effective motor control for the Pololu Metal Gearmotor family.

To start working with the DC motor, it is necessary to plug it correctly and create an object in the ``main`` file to assign DC motor output and encoder pins.

### Connection to the PES-Board
---------------------------
The DC motors have assigned pins on the PES board, and since they are equipped with encoders, the pins assigned to them are defined as follows:

```
// PES-Board Pin Names
PB_PWM_M1
PB_PWM_M2
PB_PWM_M3

PB_M1_ENC_A
PB_M1_ENC_B
PB_M2_ENC_A
PB_M2_ENC_B
PB_M3_ENC_A
PB_M3_ENC_B

PB_ENABLE_DCMOTORS
```

Pins M1 and M2 are the output of the H-Bridge, so voltage + and -. VCC and GND pins provide power to the encoder, while pins A and B are utilized to receive pulse signals from the encoder.

[PES Board pinmap](../datasheets/pes_board_peripherals.pdf)

### Enabling the Power Electronics
----------------------------------
The Nucleo F446RE microcontroller can control up to 3 DC motors using external power electronics (H-Bridge on PES-Board). Configuring the microcontroller involves setting up PWM pins, essential for adjusting the rotational speed of the motors in the external circuit.
<details Closed>
<summary><b>H-bridge and PWM</b></summary>

><center><img src="../images/hbridge_switches.png" alt="H-bridge" width="400"/></center>
><br>
> An H-bridge is a configuration of four switches that enables precise voltage control. These switches, typically transistors or MOSFETs arranged in the shape of an "H" determine the voltage and therefor the current flow through the motor. By selectively activating different pairs of switches, the H-bridge can drive the motor forward, backward, or apply braking.
><br>
><br>
><center><img src="../images/pwm.png" alt="PWM" width="600"/></center>
><br>
>Pulse Width Modulation (PWM) in DC motor control involves varying the duty cycle of a rapidly switching signal to regulate the average voltage reaching the motor. By adjusting the duty cycle of the PWM, the average voltage is adjusted accordingly. This dynamic control is achieved by applying PWM to the H-bridge, allowing bidirectional current flow through the motor for forward, reverse, and braking actions.
><br>
><br>

> - Further information about H-bridges can be found: [HERE][4]
> - Further information about PWM and DC motors can be found: [HERE][5]
</details>

<br>
To power the DC motors, connect a 12V external power source, formed by combining two 6V battery packs, to the back of the PES board. If using a 6V power supply, connect one set of pins and bridge the remaining two. Turn on the PES board using the ON/OFF slider. After turning on the power, enable the external power electronics (H-bridge) by creating a DigitalOut object and setting the digital out to 1 (or true). This object needs to be created alongside other DC motor objects.

```
DigitalOut enable_motors(PB_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors
```

To complete the motor activation process, set the value of the object to 1, enabling the use and control of the motors by utilizing battery power. Place this statement after the trigger statement to ensure it is executed in the foreground and not running in the background without purpose.

```
// enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled
enable_motors = 1;
```

### Create and command DC motor objects
---------------------------------------
The provided examples show three different used cases of a DC motor and how to use the ``DCMotor`` class. We assume that we have three DC motors and encoders that are plugged into pins M1 - M3 on the PES-Board. You can also test each used case separately when only one DC motor is available.
- [Motor M1](#motor-m1) is used Open-Loop
- [Motor M2](#motor-m2) is used Closed-Loop Velocity Control (rotations per second)
- [Motor M3](#motor-m3) is used Closed-Loop Position Control (rotations)

#### Motor M1 Open-Loop

To use Motor M1 in an open loop configuration, start by adding the PWM driver to the ``main`` file. Next, create an object by passing the pin names as arguments; these should only be PWM pins since Motor M1 is used in open loop. Additionally, define a DigitalOut object to enable the DC motor.

```
#include "pm2_drivers/FastPWM/FastPWM.h"

...

DigitalOut enable_motors(PB_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors
```

Then a FastPWM object needs to be created, which is used to command the voltage applied to the DC motor:

```
// Motor m1
FastPWM pwm_M1(PB_PWM_M1); // create FastPWM object to command motor M1
```

Motor M1 is used open-loop, meaning we just apply a certain voltage to the motor. The relation between the applied voltage and the speed is linear and the gain that maps voltage to speed is called the motor constant. The mapping is linear from (0...1.0) -> (-12V...12V):
- PWM to 0.0f then -12V is applied to the motor (assuming +/-12V are available)
- PWM to 0.5f then 0V is applied to the motor
- PWM to 1.0f then 12V is applied to the motor (assuming +/-12V are available)

A positive voltage will cause the motor to rotate in one direction and a negative voltage will cause the motor to rotate in the opposite direction. You can alter the direction by changing the cables connected to the motor.

```
pwm_M1.write(0.75f) // apply 6V to the motor (assuming +/-12V are available)
```

#### Motor M2 Closed-Loop Velocity Control

Motor M2 operates in a closed loop to control the velocity. To utilize it, it's necessary to include the appropriate driver in our ``main`` file. Then, create an object with the PWM pins, as well as the A and B pins associated with the encoder. Additionally, define a DigitalOut object to enable the DC motor. 

```
#include "pm2_drivers/DCMotor.h"        

...

DigitalOut enable_motors(PB_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors
```

When declaring a DC motor object for closed-loop control, it is essential to specify the following arguments related to motor parameters, including gear ratio, motor constant, and the maximum available voltage based on the number of battery packs. The following code illustrates the declaration of all necessary parameters to command this motor:

```
const float voltage_max = 12.0f; // maximum voltage of battery packs, adjust this to 
                                 // 6.0f V if you only use one battery pack

// motor M2
// const float gear_ratio_M2 = 78.125f;
// const float kn_M2 = 180.0f / 12.0f;
// DCMotor motor_M2(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio_M2, kn_M2, voltage_max);
```

Crucially, the default motor driver does not employ a motion planner, meaning the declared speed will be reached as quickly as possible. To test this, you can place the following command inside a while loop.

**Note:** 
- The maximum velocity is calculated and set in the driver based on the input arguments.

```
motor_M2.setVelocity(1.0f); // set setpoint to 1 rotation per second
```

Nevertheless, the driver is designed to facilitate smooth movements, incorporating features like gradual velocity increasing. The process adheres to the outlined diagram, wherein the motion planner defines trajectories. Subsequently, feedback from the motor (leveraging encoders) is obtained, and the speed controller governs the motor's movement.

<center><img src="../images/dc_motor_vel_cntrl.png" alt="DC Motor Velocity Control" width="600"/></center>
<center> <i>DC Motor Velocity Control Block Diagram</i> </center>

To be able to use the motion planner, the module needs to be activated with the following command, that is placed below the dc motor declaration:

```
motor_M2.setEnableMotionPlanner(true);
```

If there is no command to set the speed inside the while loop, add one and run the program.

```
motor_M2.setVelocity(motor_M2.getMaxVelocity()); // set setpoint to maximum velocity
```

Below are graphs of the measured velocity and acceleration versus time without and with the motion planner active. The graph on the left shows step responses without the motion planner active. The velocity overshoots the setpoint and the acceleration of the motor is larger and more abrupt. The graph on the right shows step responses with the motion planner active. The speed value reaches setpoint without any overshoot and the acceleration is smaller and bounded leading to smooth movements of the drive.

<center><img src="../images/acc_vel_graphs.png" alt="DC Motor acceleration and velocity with and without planner" width="900"/></center>
<center> <i>DC Motor velocity and acceleration with and without motion planner</i> </center>

Adjustments to the maximum acceleration can be done by using the following command (which should be placed after declaring the motor):

```
motor_M2.setMaxAcceleration(motor_M2.getMaxAcceleration() * 0.5f); // half of default acceleration
```

#### Motor M3 Closed-Loop Position Control

To utilize Motor M3 in closed loop for position control, it is crucial to include the appropriate driver in the main file and create an object with the pin's name passed as an argument. These include the PWM pins, as well as the A and B pins associated with the encoder. Additionally, a DigitalOut object should be defined to enable the DC motor.

```
#include "pm2_drivers/DCMotor.h"        

...

DigitalOut enable_motors(PB_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors
```

When defining a DC motor object intended for closed-loop control, certain parameters related to the motor characteristics (such as gear ratio and motor constant) must be specified. Additionally, the maximum available voltage needs to be defined based on the number of battery packs in use. The code snippet below illustrates how to declare these essential parameters for commanding the motor:

```
const float voltage_max = 12.0f; // maximum voltage of battery packs, adjust this to 
                                 // 6.0f V if you only use one battery pack

// motor M3
const float gear_ratio_M3 = 78.125f;
const float kn_M3 = 180.0f / 12.0f;
DCMotor motor_M3(PB_PWM_M3, PB_ENC_A_M3, PB_ENC_B_M3, gear_ratio_M3, kn_M3, voltage_max);
```

**Note:** 
- The argument passed represents the value in units of rotations.

```
motor_M3.setRotation(10.0f);
```

Nevertheless, the driver is engineered to facilitate smooth movements, including gradual velocity change. The process adheres to the illustrated diagram, wherein the motion planner establishes trajectories for the position setpoint. Subsequently, it receives position feedback from the motor (utilizing encoders for this purpose). The discrepancy is then forwarded, along with velocity feedback, to the speed controller, governing the motor's movement.

Adjustments to the maximum acceleration or maximum speed values can be made by using the following commands, which should be placed after declaring the motor.

```
motor_M3.setMaxVelocity(motor_M2.getMaxVelocity() * 0.5f); // half of maximum velocity
motor_M3.setMaxAcceleration(motor_M2.getMaxAcceleration() * 0.5f); // half of default acceleration
```

<center><img src="../images/dc_motor_control_scheme.PNG" alt="DC Motor Control Scheme" width="800"/></center>
<center> <i>DC Motor Control Block Diagram</i> </center>

To use the motion planner, the motion planner module needs to be activated with the following command, that is placed below motor declaration:

```
motor_M3.setEnableMotionPlanner(true);
```

If there is no command to set the speed inside the while loop, add one and run the program.

```
motor_M3.setPosition(10.0f);
```

In the provided graph a smooth positioning step of the DC motor is illustrated. In the first graph (upper left corner), the blue line represents the position setpoint, while the red line depicts the actual position. Notably, the red line exhibits smooth transitions at the beginning and end, indicative of uniform acceleration and deceleration. The speed graph illustrates the motor's acceleration to its maximum speed, maintaining it consistently before decelerating uniformly. This process repeats in the opposite direction. Given the direct proportionality between speed and voltage, the voltage graph mirrors the speed graph's curve. The acceleration graph showcases an initial acceleration phase, followed by quantization noise around zero when the shaft moves at a constant speed.

<center><img src="../images/dc_motor_smooth_positioning.png" alt="DC Motor Smooth Positioning" width="900"/></center>
<center> <i>Smooth Positioning of 78:1 DC Motor</i> </center>

Below are graphs of position, velocity and acceleration against time. On the right are the graphs when the motion planner is not used and on the left when the planner is active.
<center><img src="../images/pos_acc_vel_graphs.png" alt="DC Motor Positioning" width="900"/></center>
<center> <i> DC Motor position, velocity and acceleration with and without motion planner</i> </center>