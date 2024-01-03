# Analog distance sensor

|Analog distance sensor is a distance measuring sensor unit, composed of an integrated combination of PSD (position sensitive detector), IRED (infrared emitting diode) and signal processing circuit. 
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

## Analog distance sensor - instructions

> How does it work?
>
>Very briefly infrared sensors work on the principle of reflected light waves. Infrared light reflected from objects or sent from an infrared remote or beacon. Infrared sensors are also used to measure distance or proximity. The reflected light is detected and then an estimate of distance is calculated between sensor and object. The following is a pictorial representation of the principle of operation:
><center><img src="../images/how-infrared-sensors-work.png" alt="how_IR_works" width="400" /></center>
><center> <i>Principle of operation</i> </center>
>
> More detailed explanation can be found: [HERE][4]

### First steps

The first step to be able to use the IR distance sensor is to create an analog object in the object master file and define the pin to which it will be connected. In our case, it is the **PC2** pin. Map of pins can be found [HERE][5]
```
AnalogIn ir_analog_in(PC_2);
```
As we can see, there are three wires coming out of the sensor, one is used for signal transmission, another is used as ground, and the last one is used for power supply. The layout of the wires is shown below:
<center><img src="../images/ir_sensor_wires.PNG" alt="IR sensor wires" width="250" /></center>
<center> <i>Layout of wires</i> </center>

According to the documentation of the sensors, you need to power them with 5V to receive correct signals. If you are not sure how to connect the sensor, click the following hint.

<details Closed>
<summary>HINT</summary>
<br>
<center><img src="../images/connection_pin_map.png" alt="Cennection pin map" width="700" /></center>
<center> <i>Connection Pin map with marked wire's colors </i> </center>
</details>

*AnalogIn* class is a class that belongs to the libraries created by the MBed platform assigned to the board in use. It assumes a mapping of the signal received from 0...3.3V to 0...1. This should be kept in mind when using the sensor to interpret the signal correctly. The next step to use the sensor correctly is to calibrate it.

> For what we need calibration?
>
>The specifications of the sensors in question include curves describing the dependence of the recorded voltage on the measured distance.
>
><center><img src="../images/dist_measure_char.PNG" alt="Distance measuring characteristics" width="450" /></center>
><center> <i>Distance measuring characteristics</i> </center>
>These are expected values that can serve as a reference for the measurements to be made. However, all sensors, especially those of lower quality, may be characterized by a slightly altered course of the curve in question, so a calibration process should be carried out before using each sensor.

**The calibration procedure, which is also the purpose of the exercise, is presented at the end of the file.**
### Ussage
In order to use the sensor and take readings of the signals that are sent to the board, use the function:
```
ir_distance_mV = 1.0e3f * ir_analog_in.read() * 3.3f;
```
**NOTE**: Remember that the signal is mapped to a range of 0 to 1, hence the reading is multiplied times 3.3, which is the maximum range of the sensor, and times 1000 due to which the signal is converted from a unit of V to mV.

The use of the sensor after calibration is very similar, however, the measured value should be recalculated by the function obtained by calibrating the device so that it can be given in distance units such as cm or mm. Determination of such a function is necessary before each use of the sensor due to the reasons mentioned above. Therefore, carry out this process described below.

### Practical tips
* Remember that trusted measurements can only be made within the measurement range. Be especially careful near the minimum range because of its course near this point. (See Distance measuring characteristics chart)
* It should also be remembered that the principle of sensor measurement is based on the reflected light beam, so the measurement result is affected by the object, or basically its surface, from which we take the measurement. Measuring from objects with a surface that reflects light rays poorly may give incorrect results. 

## Task to do
As mentioned, the calibration process is a key part of working with the distance sensor. Before performing the exercise, you should read the previous part of the documents describing the correct connection and the features of the class that was used to receive the signal from the sensor.

### Purpose of exercise
The purpose of the exercise is to perform calibration by simultaneously measuring the distance and the corresponding voltage readings of the signal received from the sensor. Once the numerical values have been obtained, it is necessary to switch to a program (Matlab), where the results will be evaluated and the coefficients determined to describe the function that converts the signal into distance measure values. List of things needed to do the exercise can be found below:

>Hardware:
> - PES board with NUCLEO-F446RE board
> - IR sensor (check which one you have, the model name is on the side, it will determine the range of measurement)
> - Mini USB cable
> - Additional wires to connect sensor to board
> - Paper tape
> - Length measure tape
> - Mechanical button
><center><img src="../images/IR_set.png" alt="IR sensor set" width="350" /></center>
><center> <i>Hardware used in exercise</i> </center> 
>
> Software
> - PM2_Pes_board_example
> - Matlab file: eval_ir_sensor.m

### Conduct of the exercise (calibration procedure):
- First of all, you need to prepare the stand by connecting the IR sensor to the PES Board to the appropriate input pin **PC_2** (detailed instruction is show above under HINT). You should also plug the mechanical button into pin **PC_5** (map of pins can be found [HERE][5])
- Next, tape the paper tape to the floor from the edge of the wall and use a tape measure to mark the measurement points on the tape (recommended 0 to 15cm every 1cm, then 17.5 to 30cm every 2.5cm and 35 to 75cm every 5cm)
<center><img src="../images/IR_task.png" alt="IR task" width="350" /></center>
<center> <i>Performing the exercise</i> </center>

- If you connect and run the program correctly, you will see the distance sensor reading values on the serial monitor. Consider whether these values are reasonable, if not, go through the documentation again.
- The calibration process involves applying the edges of the sensor to the appropriately marked points on the tape, the sensor is to face the wall so as to measure the distance from it, try to direct the beam from the sensor parallel to the ground. At the same time, after applying to a given point, record its distance and the readout values that are indicated on the serial monitor. 
- Then, the collected points should be entered into a ([file](./eval_ir_sensor.m)) under the appropriate headings *dist_cm* and *dist_mV*. The file is used to determine the coefficients of the curve of best fit to the points. In order to get the correct result, you need to determine the appropriate range of values for which the curve is to be fitted (this range determines the minimum and maximum range of the sensor).
- The next step is to write a function to convert the sensor reading into a unit of length (cm)
```
// Function to compensate  
float ir_sensor_compensation(float _ir_distance_mV) {

    static const float a = .0f; //insert value that you got from matlab file
    static const float b = .0f; //insert value that you got from matlab file

    static float ir_distance_cm = 0.0f;

    //ir_distance_cm = YOUR FUNCTION with -ir_distance_mV as argument

    return ir_distance_cm;
}
```


<details Closed>
<summary>Hint</summary>
<br>
After writing the function, take a close look at how it is structured, are there mathematical operations in it that, with exceptional values of variables, can cause the execution of forbidden mathematical operations? If so, apply appropriate conditions to avoid this situation.
<details Closed>
<summary>Comment</summary>
<br>
Possible situation:

```
(_ir_distance_mV + b) = 0
```

In the case of this function, there is the possibility of a situation where divide by zero is performed at the corresponding sensor reading value. This situation is unlikely but theoretically possible, so it is very important to carefully analyze the operations performed by the program because dividing by zero can lead to a complete stop of the program, which in the case of a simple robot does not necessarily end up as spectacular as in the case of a drone at an altitude of 30 meters.
</details>
</details>

<details Closed>
<summary>Answear</summary>
<br>

```
float ir_sensor_compensation(float _ir_distance_mV) {

    static const float a = 2.574e+04f;
    static const float b = -29.37f;

    static float ir_distance_cm = 0.0f;
    if (_ir_distance_mV + b == 0) ir_distance_cm = -1.0f;
    else ir_distance_cm = a / (_ir_distance_mV + b);

    return ir_distance_cm;
}
```

</details>

- The next step is to uncomment the function that allows you to read the value in the cm unit when you press the button.
```
if (mechanical_button.read()) {
    ir_distance_cm = ir_sensor_compensation(1.0e3f * ir_analog_in.read() * 3.3f);
}
```

In addition, change the "printed" variable to *ir_distance_cm*:
```
printf("IR sensor (cm): %3.3f\r\n", ir_distance_cm);
```