# Ultrasonic Range Detector

An ultrasonic range detector is a type of sensor designed to measure the relative distance between the sensor itself and an object that reflects sound waves. This device operates by emitting ultrasonic pulses and measuring the time it takes for these pulses to bounce back as echoes. By knowing the speed of sound in air, it is possible to calculate the relative distance to the object based on the time elapsed between the emission of the ultrasonic pulse and the reception of its echo.

<center>
    <img src="../images/grove_ultrasonic_ranger_v2.png" alt="Grove-Ultrasonic Ranger V2.0" width="400"/>
</center>
<center>
    <i>Grove-Ultrasonic Ranger V2.0</i>
</center>

## Specifications

|                     |                 |
| ------------------- | --------------- |
| Operating voltage   | 3.2 ~ 5.2 V     |
| Measuring range     | 3 - ~200 cm     |
| Size                | 50 x 25 x 16 mm |
| Weight              | 13 g            |
| Measurment angle    | 15 °            |
| Working temperature | -10 ~ 60 °C     |

<!-- |Operating current    | 8 mA            | -->
<!-- |Ultrasonic frequency | 40 kHz          | -->
<!-- |Measuring range      | 2 - 350 cm      | -->
<!-- |Resolution           | 1 cm            | -->
<!-- |Output               | PWM             | -->
<!-- |Trigger signal       | 10 uS TTL       | -->
<!-- |Echo signal          | TTL             |  -->


**Note:**

- Because the measurements showed a relatively high variation above 2 m, the measurment range is limited to 2 m by software.

## Links

- [DigiKey][0]
- [seeed studio][1]

<!-- last updated 03.01.2024 -->
[0]: https://www.digikey.ch/de/products/detail/seeed-technology-co-ltd/101020010/5482600?s=N4IgTCBcDaIOICUDyA1AogAgKoBkAqCAggMpIByAkgMIZFlxoIYpgB0ADCALoC%2BQA
[1]: https://wiki.seeedstudio.com/Grove-Ultrasonic_Ranger/

## Practical Tips

- Do not hot plug Grove-Ultrasonic-Ranger, otherwise it will damage the sensor. The measured area must be no less than 0.5 square meters and smooth.
- Before using the sensor for responsible tasks, make sure that the measurements that are read by the sensor are correct and sufficiently accurate.

## Ultrasonic Ranger Example

The ``UltrasonicSensor`` class provides functionality to measure distances by emitting ultrasonic pulses 
and measuring the time taken for the echo to return. It encapsulates the details of interfacing with the
sensor hardware and offers a simple interface for obtaining distance measurements in centimeters.
Maximum measurment distance is approximately 2 meters (measured 198.1 cm) with a mearuement period of 12000
microseconds. If no new valid measurement is available, the read() function returns -1.0f.

### Connection to the PES-Board

The ultrasonic sensor was tested with a 5V power supply and a single pin that transmits the signal. Therefore, it can use the same ports that are intended for the servos. To start working with the sensor, it is necessary to create an object in the ``main`` file and assign a digital output. This is pin D3 located in the same row as the pins used for servos. 

You can use the following pins:

```
#define PB_D0 PB_2
#define PB_D1 PC_8
#define PB_D2 PC_6
#define PB_D3 PB_12
```
<center>
    <img src="../images/pes_board_peripherals.png" alt="board_pins" width="1000" />
</center>
<center>
    <i>PES board Pin Map</i>
</center>

<!-- TODO: The following section sounds not so great and needs to be rewritten (ask ChatGPT)
The connection to the sensor is wired, as shown in the the following image, due to the compatibility of the plug with the socket on the sensor. You can then connect a standard jumper wire to the cable shown and then use them to connect to the *PES_board*. What's more, the pins on the sensor are labeled, but the design of the plug allows you to connect in one correct way, which is that the signal is sent through the yellow wire.

<center>
    <img src="../images/Grove-Kabel.png" alt="grove kabel" width="350" />
</center>
<center>
    <i>Cable used to connect to the sensor</i>
</center> -->

### Create UltrasonicSensor Object

In the following example the sensor is pluged to the PES-Board pin D3:

```
UltrasonicSensor us_sensor(PB_D3);
```

>Details of how the algorithm works in the driver?
>
>By sending a signal to the sensor, we command it to send a pulse with a rising edge followed by a falling edge and a pulse width of 10 microseconds. The device will emit sound with 8 periods of 40 kHz frequency and try to detect the echo (via convolution or similar). The elapsed time between the rising and falling edge is proportional to the distance of the device to the object. The sensor will then send a pulse proportional to the measurment time, which will be detected by the microcontroller. The time between the rising and falling edge is measured and used to calculate the distance on the microcontroller. This process is repeated every 12000 microseconds.

<!-- #### Details of how the algorithm works in the driver
The procedure consists of the following events:
- the measurement procedure is triggered by sending a pulse, the length of which is 10 microseconds, which makes the sensor send out a sound wave. 
- This triggers a function that stops sending the pulse and waits for a rising edge
- when the rising edge appears it starts timing and waiting for the falling edge.
- the moment the falling edge appears the time is stopped, which is then used in the equation so that the distance in centimeters is obtained.:
```
m_us_distance_cm = m_gain * static_cast<float>(pulse_time) + m_offset;
``` 
The **m_gain** and **m_offset** values were obtained in the calibration process (**I GUESS**) -->

### Reading the Distance

The operation is straightforward since all processes are encapsulated within the class, yielding a completed function that returns the distance in centimeters (cm). This functionality is accessed through the following command:

```
us_distance_cm = us_sensor.read();
```

or simply

```
us_distance_cm = us_sensor;
```

If no new valid measurement is available, the read() function returns -1.0f.

**Notes:**

- Do not readout the sensor faster than every 12000 microseconds, otherwise the sensor can not work properly.
- For highly accurate measurements, every sensor unit should be calibrated individually.

