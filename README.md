<!-- 
    Styling Rules:
    - When ever possible *.md [Links] should be used, and not just bold text
    - In a *.md [Link] all words start with a capital letter, e.g.: [Course setup]
    - A button or a pin or something similar is highlited bold: **USER BUTTON**, **PB_9**
    - Source code files are highlited bold and cursive: ***main.cpp***
    - Functions, objects and variables when not in a code snippet are formatted like this:
      - ``main()`` function
      - ``servo.enable()`` function
      - ``while()`` loop
      - ``if()`` statement
      - ``mechanical_button`` object
      - ``enable_motors`` variable
      - ``DCMotor.h`` driver
    Authors:
    - Maciej Szarek (szar)
    - Peter Michael (pichim/pmic)
 -->

# PM2 PES-Board, Hardware and Driver Documentation

| Szarek Maciej | Peter Michael |
|-|-|
| szar@zhaw.ch | pmic@zhaw.ch |
| TE 302 | TE 307 |

# Table of Contents
1. [Course setup](#course-setup)
    * [Accounts](docs/markdown/general_info.md#accounts)
    * [Software](docs/markdown/general_info.md#software)
    * [GitHub](docs/markdown/general_info.md#github)
    * [Arm MBed](docs/markdown/general_info.md#arm-mbed)

2. [Hardware](#hardware)
    * [Nucleo F446RE](#nucleo-f446re)
        * [Nucleo pinmap](#nucleo-platform-pinmap)
    * [PES Board](#pes-board)
        * [Pheripherals](#pheripherals)
        * [Pin-Mapping](#pin-mapping)
    * [Hardware tutorials](#hardware-tutorials)
        * [IR Sensor](docs/markdown/ir_sensor.md)
        * [Ultrasonic Sensor](docs/markdown/ultrasonic_sensor.md)
        * [Servo](docs/markdown/servo.md)
        * [DC Motor](docs/markdown/dc_motor.md)
        * [Line Follower Array](docs/markdown/line_follower.md)
3. [Tips](#tips)
    * [Project development](docs/markdown/tips.md#project-development)
    * [Programming tips](docs/markdown/tips.md#programming-tips)
    * [Structuring a robot task](docs/markdown/tips.md#structuring-a-robot-task)

4. [Workshops](#workshops)
    * [Workshop 1](docs/markdown/ws1.md)
    * [Workshop 2](docs/markdown/ws2.md)
    * [Workshop 3](docs/markdown/ws3.md)
5. [Links](#links)

## Course setup

In order to be able to complete the course, one must first register on the following platforms and obtain the appropriate tools. All the information needed to start the course can be found [here](docs/markdown/general_info.md). The file covers all the information about: 

- [<font size="3"> Accounts </font>](docs/markdown/general_info.md#accounts)
- [<font size="3">Software </font>](docs/markdown/general_info.md#software)
- [<font size="3">GitHub </font>](docs/markdown/general_info.md#github)
- [<font size="3">Arm MBed </font>](docs/markdown/general_info.md#arm-mbed)
  
## Hardware

In our course, we'll be utilizing the Nucleo-F446RE microcontroller, which is used in combination with the PES board designed and developed at ZHAW. The basic hardwarekid includes a variety of sensors and actuators for practical applications.

### Nucleo F446RE
------------------------
The Nucleo-F446RE is a microcontroller development board featuring the STM32F446RET6 microcontroller from STMicroelectronics. It provides a versatile platform for prototyping and developing embedded systems, offering a wide range of peripherals, connectivity options, and compatibility with the Arduino and ST Morpho ecosystems. All of the technical details can be found here: [STM32F446RE Documentation][2] 

<center><img src="docs/images/nucleo_overview.png" alt="Nucleo overview" width="600" /></center>

</br>

Arm MBed provides a dedicated platform with essential information about the controller. This platform includes technical details and, crucially, the pinmap of outputs, essential for accurately connecting peripheral devices.

</br>

- [<font size="3"> NUCLEO-F446RE </font>][1]

### PES Board
------------------------
The PES Board is a hardware board created to work with the Nucleo F446RE on which appropriate outputs and inputs are designed to allow a more orderly connection of peripheral devices. It provides multiple pinouts for:

- 3 DC-Motor (brushed)
- 4 Servos (these occupy the 4 DI/O if used)
- 4 DI/O, 3.3V (5V tolerant)
- 4 AI/O, 3.3V (5V tolerant)
- 3 Encoder-Counter
- 9-axis IMU (accelerometer, gyroscope, magnetometer)

><b>Important information related to the PES Board</b>
>- <b>Using the PES-Board with power On and hardware running while connected to your computer/laptop happens at your own risk. There was a case where the students laptop was damaged in the past.</b>
>- <b>Under now circumstances leave the battery charger connected to the PES-Board when using the PES-Board or having it connected to your computer.</b>
>- <b>Motor encoder soldering can be wrong. Do not assume that if you plug in one motor and everything works you can the also use the same connections with an other motor and everything works. You have to make sure that the physical rotation is according to your assumption and that a positive input leads to a positive change of increments.</b>
>- <b>Depending on the PES BOARD version DCMotor M3 rotation direction might be inverted.</b>
<!-- >- <b>Depending on the PES BOARD version the Pin map might differ</b> -->

</br>

The kit includes two sets of 6V battery packs, which can be connected is series to provide 12 volts. The external power source is connected on the opposite side of the board. The picture below illustrates the proper battery connection required to attain a voltage of 12 volts. 
<center><img src="docs/images/battery_packs.png" alt="Battery packs" width="500" /></center>

If you are using a single battery pack, the remaining active and ground pin should be bridged.

The batteries enables the board to function independently of the computer, eliminating the need for a connection via the Mini USB cable. Despite this, the board continues to receive a stable 5V supply while offering the option to use a 12V supply for the motors. To activate the external battery power, switch the slider on the PES board to the ON position.

#### Pheripherals
------------------------
<center><img src="docs/images/pes_board_peripherals.png" alt="Nucleo overview" width="600" /></center>

</br>

PDF: [<font size="3"> pes_board_peripherals.pdf </font>](docs/datasheets/pes_board_peripherals.pdf)

#### Pin-Mapping
------------------------
PDF: [<font size="3"> pes_board_pin_mapping.pdf </font>](docs/datasheets/pes_board_pin_mapping.pdf)

### Hardware Tutorials
------------------------
The following links contain the hardware tutorials. The files structure encompass specifications and technical notes detailing the components. Following that, there's guidance on connecting to the microcontroller. The subsequent sections cover creating software objects, calibration procedures, and controlling actuators or retrieving pertinent information from sensors as needed.

- [<font size="3"> IR Sensor </font>](docs/markdown/ir_sensor.md)
- [<font size="3"> Ultrasonic Sensor </font>](docs/markdown/ultrasonic_sensor.md)
- [<font size="3"> Servo </font>](docs/markdown/servo.md)
- [<font size="3"> DC Motor </font>](docs/markdown/dc_motor.md)
- [<font size="3"> Line Follower Array </font>](docs/markdown/line_follower.md)

## Tips

All the tips that you might find useful can be found [here](docs/markdown/tips.md). In the file you can find information about: 
- [<font size="3"> Project development </font>](docs/markdown/tips.md#project-development)
- [<font size="3"> Programming tips </font>](docs/markdown/tips.md#programming-tips)
- [<font size="3"> Structuring a robot task </font>](docs/markdown/tips.md#structuring-a-robot-task)

## Workshops

The following links contain the workshops instructions:
- [<font size="3"> Workshop 1 </font>](docs/markdown/ws1.md)
- [<font size="3"> Workshop 2 </font>](docs/markdown/ws2.md)
- [<font size="3"> Workshop 3 </font>](docs/markdown/ws3.md)

## Links
Git and GitHub Tutorial for Beginners from 32:40 about Github:
https://www.youtube.com/watch?v=tRZGeaHPoaw

C++ step by step tutorials: https://www.w3schools.com/cpp/default.asp

Detailed explanation of C++ programming language: https://cplusplus.com/

C++ data types: https://www.tutorialspoint.com/cplusplus/cpp_data_types.htm

Printf format convention: https://cplusplus.com/reference/cstdio/printf/

Flowchart diagram maker: https://app.diagrams.net/

Hardware links: <br>
https://www.pololu.com​ <br>
https://www.adafruit.com​ <br>
https://www.sparkfun.com​ <br>
https://www.seeedstudio.com​ <br>
https://www.robotshop.com​ <br>
https://boxtec.ch​ <br>
https://www.play-zone.ch​ <br>
http://farnell.ch​ <br>
https://www.mouser.ch​ <br>
https://www.digikey.com​ <br>
https://www.conrad.ch​ <br>
https://www.distrelec.ch <br>


<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/
[2]: https://www.st.com/en/microcontrollers-microprocessors/stm32f446re.html#documentation 