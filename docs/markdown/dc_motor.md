# DC Motor

A direct current (DC) motor is a type of electric machine that converts electrical energy into mechanical energy. DC motors take electrical power through direct current, and convert this energy into mechanical rotation transmisioned by the electric field. DC motors use magnetic fields that occur from the electrical currents generated, which powers the movement of a rotor fixed within the output shaft. The output torque and speed depends upon both the electrical input and the design of the motor.

<center><img src="../images/dc_motor.png" alt="dc motor" width="250"/></center>
<center> <i>Example of dc motor with encoder</i> </center>

<!-- TOODO: Add 488:1 gear box motor  -->
<!-- TOODO: Maybe only include these parameters that must be adjusted or at least these and only a minimal set of the other parameters -->
## Technical Specifications
| | 31:1 Metal Gearmotor 20Dx41L mm 12V CB | 78:1 Metal Gearmotor 20Dx43L mm 12V CB |?|
|-|-|-|-|
|**Dimensions**|
|Size | 20D x 43.2L mm | 20D x 44.7L mm |-|
|Weight | 44 g| 45 g|-|
|Shaft diameter | 4 mm| 4 mm|-|
|**General specifications**|
|Gear ratio               | 31.25:1            | 78.125:1           |-|
|No-load speed @ 12V      | 450 rpm            | 180 rpm            |-|
<!-- |No-load current @ 12V    | 0.08 A             | 0.08 A             |-|
|Stall current @ 12V      | 1.6 A              | 1.6 A              |-|
|Stall torque @ 12V       | 2.4 kg·cm          | 5.4 kg·cm          |-|
|Max output power @ 12V   | 2.8 W              | 2.5 W              |-|
|No-load speed @ 6V       | 225 rpm            | 90 rpm             |-|
|Stall current @ 6V       | 0.8 A              | 0.8 A              |-|
|Stall torque @ 6V        | 1.2 kg·cm          | 2.7 kg·cm          |-|
|Motor type               | 1.6A stall @ 12V CB| 1.6A stall @ 12V CB|-|
|**Performance at maximum efficency**|
|Max efficiency @ 12V          | 39 %       | 36 %       |-|
|Speed at max efficiency       | 360 rpm    | 150 rpm    |-|
|Torque at max efficiency      | 0.44 kg·cm | 0.93 kg·cm |-|
|Current at max efficiency     | 0.35 A     | 0.33 A     |-|
|Output power at max efficiency| 1.7 W      | 1.4 W      |-| -->

## Links

[31:1 Metal Gearmotor 20Dx41L mm 12V CB][1]

[78:1 Metal Gearmotor 20Dx43L mm 12V CB][2]

[488:1 Metal Gearmotor 20Dx46L mm 12V CB][3]

<!-- link list, last updated 15.01.2023 -->
[1]: https://www.pololu.com/product/3475/specs
[2]: https://www.pololu.com/product/3477/specs
[3]: https://www.pololu.com/product/3485/specs

## Static Characteristics of the 78:1 Metal Gearmotor 20Dx43L mm 12V CB

<center><img src="../images/dc_motor_static_characteristics.png" alt="Static Characteristics of 78: DC Motor" width="700"/></center>
<center> <i>Approximate static characteristics of 78:1 DC Motor</i> </center>

<!-- TODO: Describe grafics above, and fill in bullet points below -->

- How does it change of a different gear
- How can students use this grafic to decide which motor thes use
- Describe that this is only a approximation and that the real characteristics can be different so they better have some margins when evaluating their motors

<!-- TODO: Describe magnetic encoder, an mention that we only receive relative information, so initial position will always determine zero! -->
## Encoder and relative Position

- sodifjsf
- öoisjfposifj

<!-- TODO: Describe why this may be needed? -->
## Practical Tips

- direction of movement can be changed by M1 and M2 the same with outB outA for encoder

## DC motor driver

The ``DCMotor`` class is a versatile tool designed for controlling the velocity and rotation of a DC motor with precision. It incorporates essential components like an EncoderCounter, FastPWM, Motion control, PID controller, and an IIR Filter to ensure accurate control. This class provides user-friendly methods for adjusting velocity, rotation, control gains, and obtaining the current state of the motor, offering a comprehensive set of functionalities for effective motor control for the Pololu Metal Gearmotor family.

To start working with the DC motor, it is necessary to plug it correctly and create an object in the ``main`` file to assign DC motor output and encoder pins.

### Connection to the PES-Board
---------------------------
DC motors have assigned pins on the PES board, and since they are equipped with encoders, the pins assigned to them are also defined as follows:

```
// PES-Board Pin Names
#define PB_PWM_M1 PB_13
#define PB_PWM_M2 PA_9
#define PB_PWM_M3 PA_10

#define PB_M1_ENC_A PA_6
#define PB_M1_ENC_B PC_7
#define PB_M2_ENC_A PB_6
#define PB_M2_ENC_B PB_7
#define PB_M3_ENC_A PA_0
#define PB_M3_ENC_B PA_1

#define PB_ENABLE_DCMOTORS PB_15
```

Pins M1 and M2 function as PWM pins, commanding the power electronics on the PES Board which then applies voltage on the DC Motors. VCC and GND pins provide power to the encoder, while pins A and B are utilized to receive signals from the encoder.

<!-- TODO: Add link -->
**HERE SHOULD BE HYPERLINK TO THE BOARD MAP** 

### Create DC motor object
---------------------------
<!-- TODO: Enabling the power electronics should be described seperately and beforehand -->
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
motor_M3.setVelocity(1.0f)
```

#### Motor 3
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

