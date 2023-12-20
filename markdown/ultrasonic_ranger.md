# Ultrasonic ranger
The ultrasonic range is device that is used to measure the distance between the sensor and object. Operation is based on two stages, it first emits a sound wave at a specific ultrasonic frequency. After a certain period of time in second stage, it receives the sound wave reflected by objects in the detection area.

<center><img src="grove.PNG" alt="ultrasonic ranger example" width="250"/></center>
<center> <i>Example of ultrasonic ranger</i> </center>

## Technical Specifications

| |Grove-Ultrasonic ranger V2.0|
|-|-|
|Operating voltage    | 3.2 ~ 5.2 V     |
|Operating current    | 8 mA            |
|Ultrasonic frequency | 40 kHz          |
|Measuring range      | 2 - 350 cm      |
|Resolution           | 1 cm            |
|Output               | PWM             |
|Size                 | 50 x 25 x 16 mm |
|Weight               | 13 g            |
|Measurment angle     | 15 °            |
|Working temperature  | -10 ~ 60 °C     |
|Trigger signal       | 10 uS TTL       |
|Echo signal          | TTL             | 


## Links

[Grove-Ultrasonic ranger V2.0][1]

[1]: https://www.digikey.ch/de/products/detail/seeed-technology-co-ltd/101020010/5482600?s=N4IgTCBcDaIOICUDyA1AogAgKoBkAqCAggMpIByAkgMIZFlxoIYpgB0ADCALoC%2BQA

## Ultrasonic sensor driver
Ultrasonic sensor driver is used to read the range measurments that are done by the sensor and return it in distance unit - cm.

### First steps
The ultrasonic sensor requires a 5V power supply and a single pin that transmits the signal. Therefore, it can also use the same ports that are intended for servos. To start working with the sensor, it is necessary to create an object in the *main* file and assign it a corresponding output. This is pin D3 located in the same row as the pins used for servos. 
```
// PES-Board Pin Names
#define PB_D0 PB_2
#define PB_D1 PC_8
#define PB_D2 PC_6
#define PB_D3 PB_12
```
<center><img src="board_pins.png" alt="board_pins" width="600" /></center>
<center> <i>PES board pin map</i> </center>

The connection requires the use of a cable, which is shown in the following photo, due to the compatibility with the plug with the socket on the sensor. You can then connect a standard jumper wire to the cable shown and then use them to connect to the *PES_board*. What's more, the pins on the sensor are labeled, but the design of the plug allows you to connect in one correct way, which is that the signal is sent through the yellow wire.

<center><img src="Grove-Kabel.png" alt="grove kabel" width="350" /></center>
<center> <i>Cable used to connect to the sensor</i> </center>

By default the sensor should be pluged into pin D3, enter **PB_D3** as the argument of the created object to use it.
```
UltrasonicSensor us_sensor(PB_D3);
```
>How does it work?
>
>By sending a signal to the sensor, we make it send a pulse with a rising edge followed by a falling edge. With more than 10uS through a single pin, Ultrasonic Ranger will emit 8 cycles of 40kHz and detect the echo. The elapsed time between the rising and falling edge is proportional to the distance of the
> 

#### Details of how the algorithm works in the driver
The procedure consists of the following events:
- the measurement procedure is triggered by sending a pulse, the length of which is 10 microseconds, which makes the sensor send out a sound wave. 
- This triggers a function that stops sending the pulse and waits for a rising edge
- when the rising edge appears it starts timing and waiting for the falling edge.
- the moment the falling edge appears the time is stopped, which is then used in the equation so that the distance in centimeters is obtained.:
```
m_us_distance_cm = m_gain * static_cast<float>(pulse_time) + m_offset;
``` 
The **m_gain** and **m_offset** values were obtained in the calibration process (**I GUESS**)

### Ussage
The usage is very simple due to the fact that all operations take place inside the class and a finished function is returned in the unit of distance - cm. This is done using the following command:
```
us_distance_cm = us_sensor.read();
```

### Practical tips
- Do not hot plug Grove-Ultrasonic-Ranger, otherwise it will damage the sensor. The measured area must be no less than 0.5 square meters and smooth.
- Before using the sensor for responsible tasks, make sure that the measurements that are read by the sensor are correct and sufficiently accurate.

