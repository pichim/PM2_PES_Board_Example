# Analog distance sensor

The analog distance sensor which is equipped with IR diode utilizes triangulation to accurately measure distances. Using infrared technology, it calculates distances by measuring angles, establishing precise spatial relationships. This device emits infrared light, observes reflections, and provides reliable real-time distance measurements. Valuable for tasks requiring precise distance determination, it serves as a versatile solution for applications like navigation, mapping, and object localization.


<details Closed>
<summary><b>How does it work?</b></summary>

>Very briefly infrared sensors work on the principle of reflected light waves. Infrared light reflected from objects or sent from an infrared remote or beacon. Infrared sensors are also used to measure distance or proximity. The reflected light is detected and then an estimate of distance is calculated between sensor and object. The following is a pictorial representation of the principle of operation:
><center><img src="../images/how-infrared-sensors-work.png" alt="how_IR_works" width="400" /></center>
><center> <i>Principle of operation</i> </center>
>
> More detailed explanation can be found: [HERE][4]

</details>

<center><img src="../images/ir-distance-sensor.png" alt="IR_sensor" width="250" /></center>
<center> <i>Example of IR distance sensor</i> </center>

## Technical Specifications
| | | | Sharp GP2D120 | Sharp GP2Y0A21YK0F | Sharp GP2Y0A02YK0F|
|-|-|-|-|-|-|
| |Symbol|Conditions| | | |
|**Absolute Maximum Ratings**       |    |Ta=25 °C, Vcc = 5 VDC|                                    |                                    |                          |
|Supply Voltage                     |Vcc |                     |-0.3 to +7 V                        |-0.3 to +7 V                        |-0.3 to +7 V              |
|Output Terminal Voltage            |Vo  |                     |-0.3 to (Vcc +0.3) V                |-0.3 to (Vcc +0.3) V                |-0.3 to (Vcc +0.3) V      |
|Operating Temperature              |Topr|                     |-10 to +60 °C                       |-10 to +60 °C                       |-10 to +60 °C             |
|Storage Temperature                |Tstg|                     |-40 to +70 °C                       |-40 to +70 °C                       |-40 to +70 °C             |
|**Operating Supply Voltage**                                                                                                               |
|Supply Voltage                     |Vcc |                     |4.5 to 5.5 V                        |4.5 to 5.5 V                        |4.5 to 5.5 V              |
|**Electro-optical Characteristics**|    |Ta=25 °C, Vcc = 5 VDC|                                    |                                    |                          |
|Measuring Distance Range           |ΔL  |                     |(MIN) 4.00 (TYP) ---- (MAX) 30.0 cm |(MIN) 10.0 (TYP) ---- (MAX) 30.0 cm |(MIN) 20.0 (TYP) ---- (MAX) 150 cm |
|Output Terminal Voltage            |Vo  |at max range         |(MIN) 0.25 (TYP) 0.40 (MAX) 0.55 V  |(MIN) 0.25 (TYP) 0.40 (MAX) 0.55 V  |(MIN) 0.25 (TYP) 0.40 (MAX) 0.55 V |
|Output Terminal Difference         |ΔVo |between range limits |(MIN) 1.95 (TYP) 2.25 (MAX) 2.55 V  |(MIN) 1.65 (TYP) 1.90 (MAX) 2.15 V  |(MIN) 1.80 (TYP) 2.05 (MAX) 2.30 V |
|Average Supply Current             |Icc |at max range         |(MIN) ---- (TYP) 33.0 (MAX) 50.0 mA |(MIN) ---- (TYP) 30.0 (MAX) 40.0 mA |(MIN) ---- (TYP) 33.0 (MAX) 50.0 mA |


## Links

[Sharp GP2D120][1]

[Sharp GP2Y0A21YK0F][2]

[Sharp GP2Y0A02YK0F][3]

<!-- link list, last updated 07.12.2023 -->
[1]: https://www.pololu.com/product/1136
[2]: https://www.pololu.com/product/136
[3]: https://www.pololu.com/product/1137
[4]: https://robocraze.com/blogs/post/ir-sensor-working
[5]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/

## Practical Tips
* Remember that trusted measurements can only be made within the measurement range. Be especially careful near the minimum range because of its course near this point (See distance measuring characteristics chart in technical documentation of the sensor).
* It should also be remembered that the principle of sensor measurement is based on the reflected light beam, so the measurement result is affected by the object, or basically its surface, from which we take the measurement. Measuring from objects with a surface that reflects light rays poorly may give incorrect results. 

## Analog distance sensor
The ``AnalogIn`` class is a class that belongs to the libraries created by the MBed platform assigned to the board in use. It assumes a mapping of the signal received from 0...3.3V to 0...1. This should be kept in mind when using the sensor to interpret the signal correctly.

To start working with the sensor, it is necessary to plug it correclty and create an object in the ``main`` file and assign a analog input.

### Connection to the Nucleo-Board
According to the sensor documentation, it is necessary to power the sensors with 3.3V to ensure the reception of accurate signals. It is evident that three wires emerge from the sensor: one for signal transmission, another for ground, and the last one for power supply. The transmission wire should be connected to the pin that allows the reception of an analog signal, in our example this is the **PC_2** pin.

**HERE SHOULD BE HYPERLINK TO THE BOARD MAP** 

If you are not sure how to connect the sensor, click the following hint.

<details Closed>
<summary>HINT</summary>
<br>
<center><img src="../images/connection_pin_map.png" alt="Cennection pin map" width="700" /></center>
<center> <i>Connection Pin map with marked wire's colors </i> </center>
</details>

### Create analog distance sensor object
As previously mentioned, the transmission wire of the sensor is connected to pin **PC_2** on the Nucleo-Board. The appropriate driver, responsible for reading values from the sensor, is already included in the mbed drivers declared at the beginning of the ``main`` file. Proceed to create an object with the pin's name passed as an argument and define a variable to capture the corresponding value read from the sensor.
```
float ir_distance_mV = 0.0f;
AnalogIn ir_analog_in(PC_2);
```
### Calibration
The next step to use the sensor correctly is to calibrate it.

<details Closed>
<summary><b>For what we need calibration?</b></summary>

>The specifications of the sensors in question include curves describing the dependence of the recorded voltage on the measured distance.
>
><center><img src="../images/dist_measure_char.png" alt="Distance measuring characteristics" width="450" /></center>
><center> <i>Distance measuring characteristics</i> </center>
>These are expected values that can serve as a reference for the measurements to be made. However, all sensors, especially those of lower quality, may be characterized by a slightly altered course of the curve in question, so a calibration process should be carried out before using each sensor.

</details>

The procedure involves calibrating by simultaneously measuring the distance and the corresponding voltage readings of the signal received from the sensor. Once the numerical values have been obtained, it is necessary to switch to a program (Matlab), where the results will be evaluated and the coefficients determined to describe the function that converts the signal into distance measure values. List of things needed to do the exercise can be found below:

>Hardware:
> - NUCLEO-F446RE board
> - IR sensor (check which one you have, the model name is on the side, it will determine the range of measurement)
> - Mini USB cable
> - Additional wires to connect sensor to board
> - Paper tape
> - Length measure tape
><center><img src="../images/IR_set.png" alt="IR sensor set" width="350" /></center>
><center> <i>Hardware used in exercise</i> </center> 
>
> Software
> - Matlab file: eval_ir_sensor.m ZROBIĆ HIPERŁĄCZE

#### Procedure
- Tape the paper tape to the floor from the edge of the wall and use a tape measure to mark the measurement points on the tape (recommended 0 to 15cm every 1cm, then 17.5 to 30cm every 2.5cm and 35 to 75cm every 5cm)
<center><img src="../images/IR_task.png" alt="IR task" width="350" /></center>
<center> <i>Performing the exercise</i> </center>

- To read the values measured by the sensor, it is essential to include a command that will be executed in every iteration of the program. Therefore, this command is positioned within the *while* loop. Location of the command will start sensor reading after starting the program execution with the **USER** button.
```
ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;
```
- To obtain printouts on the serial monitor, insert the relevant command
```
printf("IR distance mV: %f \n", ir_distance_mV);
```
- Once the above commands are written, the next step is to compile and run it. This process serves to validate the proper functioning of the program and ensure the accurate reception of sensor readings.
- During the calibration process, position the sensor's edge at marked points on the tape. The sensor should face the wall to measure the distance from it, and it's important to align the sensor's beam parallel to the ground. Simultaneously, note the distance and the corresponding readout values displayed on the serial monitor after applying it to each designated point.
- After collecting the data points, input them into a ([file](./eval_ir_sensor.m)) under the respective headings dist_cm and dist_mV. This file aids in determining the coefficients for the best-fit curve. To achieve accurate results, it's crucial to define the suitable range of values for the curve fitting, as this range establishes the sensor's minimum and maximum range.
- Following this, proceed to create a function that converts the sensor readings into a unit of length (cm). While the function definition can be positioned at the end of the ``main`` file, it must be declared before *main* function to ensure successful compilation.
```
// Function definition (at the end of the file)
float ir_sensor_compensation(float _ir_distance_mV) {

    static const float a = .0f; //insert value that you got from matlab file
    static const float b = .0f; //insert value that you got from matlab file

    static float ir_distance_cm = 0.0f;

    ir_distance_cm = YOUR FUNCTION with -ir_distance_mV as argument

    return ir_distance_cm;
}
```
<details Closed>
<summary><b>Note</b></summary>
<br>
After writing the function, take a close look at how it is structured, are there mathematical operations in it that, with exceptional values of variables, can cause the execution of forbidden mathematical operations? If so, apply appropriate conditions to avoid this situation.
<details Closed>
<summary><b>Mentioned situation</b></summary>
<br>
Possible situation:

```
(_ir_distance_mV + b) = 0
```

In the case of this function, there is the possibility of a situation where divide by zero is performed at the corresponding sensor reading value. This situation is unlikely but theoretically possible, so it is very important to carefully analyze the operations performed by the program because dividing by zero can lead to a complete stop of the program, which in the case of a simple robot does not necessarily end up as spectacular as in the case of a drone at an altitude of 30 meters.
</details>
</details>

```
//Function declaration (at the beginning)
float ir_sensor_compensation(float _ir_distance_mV);
```
- To read the distance in centimeters, declare the variable that will handle this value in the same location where the variable to handle the value in millivolts is declared.
```
float ir_distance_cm = 0.0f;
```
- Following this, proceed to call the function for evaluation within the *while* loop.
```
ir_distance_cm = ir_sensor_compensation(ir_distance_mV);
```
- Finally, add the new variable to the printing command as the last step.
```
printf("IR distance mV: %f IR distance cm: %f \n", ir_distance_mV, ir_distance_cm);
```
- Finally, in order to return these initial values without having to start the program again, reset all values, which will be done after pressing the **USER** button.
```
led1 = 0;
ir_distance_mV = 0.0f;
ir_distance_cm = 0.0f;
```

### Read the measured distance
To utilize the sensor and capture readings of the signals sent to the board, employ the function:
```
ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;
```
**Notes:**
- Keep in mind that the signal is mapped to a range of 0 to 1. Consequently, the reading is multiplied by 3.3, representing the maximum range of the sensor, and then by 1000 to convert the signal from volts to millivolts.

After calibration, employing the sensor is quite similar. However, the measured value needs to be recalculated using the function acquired during the calibration process. This ensures the result is presented in distance units such as centimeters or millimeters. Determining this function is essential before each use of the sensor for the reasons explained earlier.
```
ir_distance_cm = ir_sensor_compensation(ir_distance_mV);
```






