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

## DC motor driver
The DC motor controller is designed to control the speed of a DC motor by entering values in revolutions per second. In addition, it is also possible to declare the rotation value. The driver includes a number of functions for reading parameters, which will be discussed later, but also functions for imposing constraints on movement so as to achieve smooth movements.

### First steps
First step to use the DC motor driver is to create in the main file the DC motor object and define the pins that will be assigned to the object. DC motor are assigned to PES board pins M1 to M3.

Defining PWM pins:
```
#define PB_PWM_M1 PB_13
#define PB_PWM_M2 PA_9
#define PB_PWM_M3 PA_10
```
Defining encoder pins:
```
#define PB_M1_ENC_A PA_6
#define PB_M1_ENC_B PC_7
#define PB_M2_ENC_A PB_6
#define PB_M2_ENC_B PB_7
#define PB_M3_ENC_A PA_0
#define PB_M3_ENC_B PA_1
```
<center><img src="../images/board_pins.png" alt="board_pins" width="600" /></center>
<center> <i>PES board pin map</i> </center>

When declaring a DC motor object, it is necessary to specify three arguments related to the pins to which the motor is connected. This is the PWM pin, and the A and B pins belonging to the encoder. The following will show the code used to declare motor:
```
const float gear_ratio_M1 = 31.25f;                        // 31.25:1 gear box
const float kn_M1 = 450.0f / 12.0f;                        // motor constant in RPM / V
DCMotor motor_M1(PB_PWM_M1, PB_ENC_A_M1, PB_ENC_B_M1, gear_ratio_M1, kn_M1, voltage_max);
const float velocity_max_M1 = kn_M1 * voltage_max / 60.0f; // maximum velocity in rotations per second
motor_M1.setMaxVelocity(velocity_max_M1 * 0.5f);
```

DigitalOut enable_motors(PN_ENABLE_DCMOTORS); // create DigitalOut object to enable dc motors