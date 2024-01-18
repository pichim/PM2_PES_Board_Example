# DC Motor

A direct current (DC) motor is a type of electric machine that converts electrical energy into mechanical energy. DC motors take electrical power through direct current, and convert this energy into mechanical rotation transmisioned by the electric field. DC motors use magnetic fields that occur from the electrical currents generated, which powers the movement of a rotor fixed within the output shaft. The output torque and speed depends upon both the electrical input and the design of the motor.

<center><img src="../images/dc_motor.png" alt="dc motor" width="250"/></center>
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

## Datasheets

[20D Pololu Motors](../datasheets/pololu-20d-metal-gearmotors.pdf)

## Static Characteristics of the 78:1 Metal Gearmotor 20Dx43L mm 12V CB

<center><img src="../images/dc_motor_static_characteristics.png" alt="Static Characteristics of 78: DC Motor" width="700"/></center>
<center> <i>Approximate static characteristics of 78:1 DC Motor</i> </center>

- These characteristics are approximations, and actual motors models may exhibit variations. It is advisable to make judicious choices in selecting motors, incorporating safety margins to account for these inaccuracies and avoid operating the motor at its extreme parameters. The top-left graph demonstrates the correlation between increasing torque and amperage, while the graph on the right illustrates the relationship between increasing speed and voltage. The subsequent graphs integrate these characteristics, revealing the interplay between torque and speed, aiding in motor selection. Additionally, the bottom-right graph highlights that the maximum power is generated within a specific range of speed and torque.
- The selection of different gear ratios significantly influences the capabilities of the engine. While the voltage and amperage ranges remain constant, adjustments in gear ratios impact torque and speed values. Opting for a larger gear ratio like 31:1 yields higher speeds at the expense of reduced available torque. Conversely, a lower gear ratio, such as 488:1, results in a comparatively lower maximum speed but offers significantly higher torque.
- During the design process, leveraging the presented characteristics proves beneficial for the appropriate motor selection. When aiming for high speeds, emphasizing the speed-voltage relationship is crucial, while lifting heavy objects places greater importance on torque characteristics. Ultimately, a judicious compromise can be achieved by considering the torque-speed relationship, enabling the optimal matching of the motor to the specific design requirements.

## Encoder and relative positioning
- A magnetic encoder is a sensor device that utilizes magnets and sensors to measure the position of a rotating object. It typically consists of a magnetically encoded disk attached to the rotating part and a sensor that detects changes in the magnetic field, converting them into electrical signals. It provides information about the rotational position of the object.
- It's important to note that magnetic encoders typically provide relative position information. This means that the encoder only gives data about changes in position relative to a specific reference point, and the initial position upon power-up or system initialization is considered as the zero reference point. Therefore, the accuracy of the measurements relies on the consistency of this reference, and any absolute positioning requires additional measures or sensors.

## Practical Tips

- To alter the motor's movement direction, adjust the power supply connection to the M1 and M2 outputs accordingly, and do the same for the encoder using the outA and outB outputs. It's crucial to note that the speed controller functions accurately when the encoder readings align with the motor's rotation direction. To ensure proper operation, establish the positive direction first, then connect the components to ensure both the direction and encoder readings are positive.
## DC motor driver

The ``DCMotor`` class is a versatile tool designed for controlling the velocity and rotation of a DC motor with precision. It incorporates essential components like an EncoderCounter, FastPWM, Motion control, PID controller, and an IIR Filter to ensure accurate control. This class provides user-friendly methods for adjusting velocity, rotation, control gains, and obtaining the current state of the motor, offering a comprehensive set of functionalities for effective motor control for the Pololu Metal Gearmotor family.

To start working with the DC motor, it is necessary to plug it correctly and create an object in the ``main`` file to assign DC motor output and encoder pins.

### Connection to the PES-Board
---------------------------
DC motors have assigned pins on the PES board, and since they are equipped with encoders, the pins assigned to them are also defined as follows:

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

Pins M1 and M2 function as PWM pins, commanding the power electronics on the PES Board which then applies voltage on the DC Motors. VCC and GND pins provide power to the encoder, while pins A and B are utilized to receive signals from the encoder.

[PES Board pinmap](../datasheets/pes_board_peripherals.pdf)



### Enabling power electronics
---------------------------
The microcontroller commands DC motors through external power electronics, specifically an H-bridge for motor control. To interface and manage this external circuit, the microcontroller must be configured by setting up GPIO pins, timers, and PWM, the latter being crucial for adjusting the rotational speed of the motors.
<details Closed>
<summary><b>H-bridge and PWM</b></summary>

><center><img src="../images/hbridge_switches.png" alt="H-bridge" width="700"/></center>
> An H-bridge is a configuration of four switches that enables precise control over the direction and speed of a motor. These switches, typically transistors or MOSFETs arranged in the shape of an "H," determine the current flow through the motor. By selectively activating different pairs of switches, the H-bridge can drive the motor forward, backward, or apply braking. Pulse Width Modulation (PWM) is often employed for speed control, allowing the regulation of motor speed by adjusting the ON and OFF times of the switches. 

><center><img src="../images/pwm.png" alt="PWM" width="700"/></center>
>Pulse Width Modulation (PWM) in DC motor control involves varying the duty cycle of a rapidly switching signal to regulate the average voltage reaching the motor. By adjusting the duty cycle, the speed of the motor can be finely controlled, with higher duty cycles resulting in increased speed and vice versa. This dynamic control is achieved by applying PWM to the H-bridge, allowing bidirectional current flow through the motor for forward, reverse, and braking actions.
><br>
><br>

> More about H-bridge: [HERE][4]
>
> More abour PWM in DC motors [HERE][5]
</details>

To power the DC motors, connect a 12V external power source, formed by combining two 6V battery packs, to the back of the PES board. If using a 6V power supply, connect one set of pins and bridge the remaining two. Turn on the PES board using the ON/OFF slider. After turning on the power, enable the external power electronics, represented by an H-bridge, by creating a DigitalOut object with a PIN argument that enables the motors. This object should be created alongside other DC motor objects.
```
DigitalOut enable_motors(PB_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors
```
To complete the motor activation process, set the value of the object to 1, enabling the use and control of the motors by utilizing battery power. Place this statement after the trigger statement to ensure it is executed in the foreground and not running in the background without purpose.
```
// enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled
enable_motors = 1;
```

### Create DC motor object
---------------------------
In the given example, DC motors and encoders are plugged into pins M1 - M3 on the PES-Board. Initially, it's essential to add the suitable driver to our main file and then create an object with the pin's name passed as an argument, these are the PWM pins, and the A and B pins belonging to the encoder. There should be also DigitalOut object definied to enable DC motors. What is more, the motors will can used in different way:
- Motor M1 is used open-loop,
- Motor M2 is used closed-loop to command velocity (rotations per second),
- Motor M3 is used closed-loop to command position (rotations),

When declaring a DC motor object, it is necessary to specify two arguments related to the motor parameters (gear ratio, motor constant) and also the maximum voltage available depending on the number of battery packs in use. The maximum velocity and acceleration are calculated and set in the driver based on these input arguments. The following will show the code used to declare all necessary objects and different motors:

<!-- TODO: Change the example below, and add a bullet point list afterwards with a description.
In general the dcmotor harddware description needs some love! Maybe think about creating WS3 first and then based on this finalise the hardware description. -->
```
#include "pm2_drivers/EncoderCounter.h" // used for open-loop control
#include "pm2_drivers/DCMotor.h"        // used for closed-loop speed and position control

...

DigitalOut enable_motors(PB_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors

const float voltage_max = 12.0f;     // maximum voltage of battery packs, adjust this to 
                                     // 6.0f V if you only use one batterypack

//Motor M1
FastPWM pwm_M1(PB_PWM_M1);                           // create FastPWM object to command motor M1

//Motor M2
// https://www.pololu.com/product/3485/specs
const float gear_ratio_M2 = 488.28125f;                    // 488.28125:1 gear box
const float kn_M2 = 28.0f / 12.0f;                         // motor constant in RPM / V
DCMotor motor_M2(PB_PWM_M2, PB_ENC_A_M2, PB_ENC_B_M2, gear_ratio_M2, kn_M2, voltage_max);
motor_M2.setMaxVelocity(motor_M2.getMaxVelocity() * 0.5f);
motor_M2.setMaxAcceleration(motor_M2.getMaxAcceleration() * 0.5f);
// const float velocity_max_M2 = kn_M2 / 60.0f * voltage_max; // maximum velocity in rotations per second
// motor_M2.setMaxVelocity(velocity_max_M2 * 0.5f);           // set maximum velocity to 50% of maximum velocity

//Motor M3
// https://www.pololu.com/product/3477/specs
const float gear_ratio_M3 = 78.125f;                       // 78.125:1 gear box
const float kn_M3 = 180.0f / 12.0f;                        // motor constant in RPM / V
DCMotor motor_M3(PB_PWM_M3, PB_ENC_A_M3, PB_ENC_B_M3, gear_ratio_M3, kn_M3, voltage_max);
motor_M3.setMaxVelocity(motor_M3.getMaxVelocity() * 0.5f);
motor_M3.setMaxAcceleration(motor_M3.getMaxAcceleration() * 0.5f);
// const float velocity_max_M3 = kn_M3 / 60.0f * voltage_max; // maximum velocity in rotations per second
// motor_M3.setMaxVelocity(velocity_max_M3 * 0.5f);           // set maximum velocity to 50% of maximum velocity
```

### Command the DC motor
---------------------------
Regardless of how the motors are employed, the initial action is to activate them using the specified command. This command should be placed at the point where the program execution begins, for instance, triggered by pressing the USER button. Ensure that the subsequent statement is positioned after the trigger to prevent it from running aimlessly in the background.

```
// enable hardwaredriver dc motors: 0 -> disabled, 1 -> enabled
enable_motors = 1;
```

#### Motor M1
---------------------------
Motor M1 is commanded directly with PWM to set their speed and direction of rotation. In the case of this motor, there is no motor motion control, so the encoder is not used in this case. This causes the set speed to be obtained, with following command by defining only a value between 0 and 1, where 0.5 is the value at which the motor does not move. Below 0.5 the motor spins in one of the directions, above 0.5 it spins in the opposite direction.

```
pwm_M1.write(0.75f)
```

#### Motor M2
---------------------------
Motor M2 is commanded in a closed-loop to set a desired speed for speed controlled dc motors. The desired speed is set through following command that take the value in rotations per second.

```
motor_M2.setVelocity(1.0f)
```

#### Motor M3
---------------------------
Motor M3 is commanded in a closed-loop to set a desired position for position controlled dc motors. The desired rotation is set through following command that take the value in degrees.

```
motor_M3.setRotation(10.0f)
```

<!-- TODO: Use those graphics, maybe go check out workshop_1 and workshop_2 slides from last year again : - ) -->

<center><img src="../images/dc_motor_control_scheme.PNG" alt="DC Motor Control Scheme" width="900"/></center>
<center> <i>DC Motor Control Block Diagram</i> </center>

<!-- TODO: Remove this, this is just an example of how to force some additional lines -->
<br>
<br>
<br>

<center><img src="../images/dc_motor_smooth_positioning.png" alt="DC Motor Smooth Positioning" width="900"/></center>
<center> <i>Smooth Positioning of 78:1 DC Motor</i> </center>

