# Workshop 1

## Scope of Workshop 1

The first workshop aims to introduce the Nucleo-F446RE microcontroller. We will familiarize ourselfs with the Mbed Studio development environment, the chosen platform for coding and software/firmware development. The workshop will start by delving into the construction of the **main.cpp** file, its structure, and the implementation of essential commands. Subsequently, participants will explore hardware utilization, specifically the IR Sensor, and its calibration, a process facilitated by the use of Matlab.

## Hardware

>Part 1:
> - NUCLEO-F446RE board
> - Mini USB cable for flashing and serial communication
> - LED diode
> - Jumper wires

>Part 2:
> - NUCLEO-F446RE board
> - Mini USB cable
> - IR sensor (check which one you have in your kid, the model name is on the side, it will determine the maximum range of suported measurements)
> - Additional wires to connect the sensor to the NUCLEO board
> - Paper tape
> - Length measuring tape

## Assignment

We assume that you already have all necessary accounts and software to conduct the workshop. If not, go to: [course setup](../markdown/general_info.md), and follow the instructions there.

Furthermore, ensure that you are familiar with the general structure of the board. If not, refer to: [Nucleo F446RE](../../README.md#nucleo-f446re)

### Part 1
------------------
The first task will be to run a test program using an additional diode.

1. Connect the LED to the pin PB_9 and ground to the corresponding pin (see [Nucleo Board pinmap][1])
2. After connecting the additional LED, connect the board to the computer using the mini USB cable. Immediately after connecting, a console window should appear in Mbed studio.
3. Make sure you see the active program and the connection to the board in the upper left corner of the screen. If so, you can flash the compiled programm to the microcontroller using the ``Play`` button.
4. Monitor the progress in the bottom left corner of the screen, where windows will display information about the uploading process. The LD1 LED will blink with green and red lights during uploading and stay continuously lit once the process is complete.
5. Now click the **USER** button while watching the LED.
6. Now click the **USER** button once again, what happend with LED diode?
7. Unplug the Nucleo board.

After finishing the task try to read through the code with the comments. Observe how the code structure looks like and how individual objects are defined.

**If you have any questions, just ask them : - ), always!**

### Part 2
------------------
The second task will be carried out using an IR distance sensor, and the main goal will be to calibrate the sensor so that it can be used to measure distances in centimeters.

1. Read the IR sensor manual, the calibration process is described there step by step. There you can alo find technical infromation about sensor and instructions for connecting to the board. Perform the calibration according to the instruction:

    > [IR sensor tutorial](../markdown/ir_sensor.md)
## Summary

In this workshop, participants were introduced to the Nucleo-F446RE microcontroller, acquiring familiarity with the Mbed Studio environment for coding. The workshop included the understanding of the ``main`` file, comprehension of its structure, defining and working with objects and the implementation of essential commands. Additionally, exploration of hardware utilization, particularly the IR sensor, and calibration using Matlab were conducted. This foundational workshop aimed to put first steps in using the microcontroler, laying the groundwork for further exploration in robotics and mechatronics.

Questions for own consideration:
<!-- TODO think about it, about those question -->
- THINK ABOUT IT

## Solution

[Workshop 1](../solutions/main_ws1.txt)

<center><img src="../images/ir_sensor_eval.png" alt="IR sensor evaluation" width="400" /></center>
<center> <i>IR sensor evaluation graph</i> </center>


<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/

