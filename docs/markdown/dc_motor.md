# DC Motor
A direct current (DC) motor is a type of electric machine that converts electrical energy into mechanical energy. DC motors take electrical power through direct current, and convert this energy into mechanical rotation. DC motors use magnetic fields that occur from the electrical currents generated, which powers the movement of a rotor fixed within the output shaft. The output torque and speed depends upon both the electrical input and the design of the motor.

<center><img src="../images/dc_motor.png" alt="dc motor" width="250"/></center>
<center> <i>Example of dc motor with encoder</i> </center>

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
|No-load current @ 12V    | 0.08 A             | 0.08 A             |-|
|Stall current @ 12V      | 1.6 A              | 1.6 A              |-|
|Stall torque @ 12V       | 2.4 kg·cm          | 5.4 kg·cm          |-|
|Max output power @ 12V   | 2.8 W              | 2.5 W              |-|
|No-load speed @ 6V       | 225 rpm            | 90 rpm             |-|
|Stall current @ 6V       | 0.8 A              | 0.8 A              |-|
|Stall torque @ 6V        | 1.2 kg·cm          | 2.7 kg·cm          |-|
|Extended motor shaft?    | N                  | N                  |-|
|Long-life carbon brushes?| Y                  | Y                  |-|
|Motor type               | 1.6A stall @ 12V CB| 1.6A stall @ 12V CB|-|
|**Performance at maximum efficency**|
|Max efficiency @ 12V          | 39 %       | 36 %       |-|
|Speed at max efficiency       | 360 rpm    | 150 rpm    |-|
|Torque at max efficiency      | 0.44 kg·cm | 0.93 kg·cm |-|
|Current at max efficiency     | 0.35 A     | 0.33 A     |-|
|Output power at max efficiency| 1.7 W      | 1.4 W      |-|

## Links

[31:1 Metal Gearmotor 20Dx41L mm 12V CB][1]

[78:1 Metal Gearmotor 20Dx43L mm 12V CB][2]

<!-- link list, last updated 07.12.2023 -->
[1]: https://www.pololu.com/product/3475
[2]: https://www.pololu.com/product/3477
[3]: 
[4]: 

## Practical Tips
- direction of movement can be changed by M1 and M2 the same with outB outA for encoder



## DC motor driver
The ``DCMotor`` class is a versatile tool designed for setting and managing the velocity and rotation of a DC motor with precision. It incorporates essential components like an EncoderCounter, FastPWM, Motion control, PID controller, and an IIR Filter to ensure accurate control. This class provides user-friendly methods for adjusting velocity, rotation, control gains, and obtaining the current state of the motor, offering a comprehensive set of functionalities for effective motor control.

To start working with the DC motor, it is necessary to plug it correclty and create an object in the ``main`` file to assign DC motor output and encoder pins.



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
```

Pins M1 and M2 function as PWM pins, supplying the motor with electrical energy to generate motion. VCC and GND pins provide power to the encoder, while pins A and B are utilized to receive signals from the encoder.

**HERE SHOULD BE HYPERLINK TO THE BOARD MAP** 

### Create DC motor object
---------------------------
In the given example, DC motors and encoders are plugged into pins M1 - M3 on the PES-Board. Initially, it's essential to add the suitable driver to our main file and then create an object with the pin's name passed as an argument, these are the PWM pins, and the A and B pins belonging to the encoder. There should be also DigitalOut object definied to enable DC motors. What is more, the motors will can used in different way:
- motor M1 is used open-loop,
- motor M2 is used closed-loop to command velocity (rotations per second),
- motor M3 is used closed-loop to command position (rotations),

when declaring a DC motor object, it is necessary to specify two arguments related to the motor parameters (gear ratio, motor constant) and also maximum voltage of battery packs. Additionally there is maximum velocity and acceleration callculated and set. The following will show the code used to declare all necessary objects and different motors:
```
#include "pm2_drivers/EncoderCounter.h"
#include "pm2_drivers/DCMotor.h"

...
DigitalOut enable_motors(PN_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors

const float voltage_max = 12.0f;     // maximum voltage of battery packs, adjust this to 6.0f V if you only use one batterypack
 
AS FAR AS I REMEMBER STUDENTS HAVE ACCESS TO ONLY ONE ROBOT WITH ONLY SAME GEAR RATIO??????

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
Regardless of how the motors are employed, the initial action is to activate them using the specified command. This command should be placed at the point where program execution begins, for instance, triggered by pressing the USER button. Ensure that the subsequent statement is positioned after the trigger to prevent it from running aimlessly in the background.
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

