# Workshop 1

## Purpose of workshop 1
The first workshop aims to introduce to the Nucleo-F446RE microcontroller and a customized PES board. Additionally, it will familiarize with the Mbed Studio environment, the chosen platform for coding and software development. The workshop will start by delving into the construction of the "main" file, its structure, and the implementation of essential commands. Subsequently, participants will explore hardware utilization, specifically the IR diode, and its calibration, a process facilitated by the use of Matlab software.

## Hardware
>First part:
> - NUCLEO-F446RE board
> - Mini USB cable
> - LED diode
> - Jumper wires

>Second part:
> - NUCLEO-F446RE board
> - Mini USB cable
> - IR sensor (check which one you have, the model name is on the side, it will determine the range of measurement)
> - Additional wires to connect the sensor to the NUCLEO board
> - Paper tape
> - Length measure tape

## Assignment
We assume that you already have all the accounts and software necessary to conduct the class if not, go to: [course setup](../markdown/general_info.md), and follow the instructions there. What is more you also know the general structure of the board, if not, go to: [Nucleo F446RE](../../README.md#nucleo-f446re)
### First part
------------------
The first task will be to run a test program using an additional diode.

1. Connect the LED to the PB_9 pin and the ground to the corresponding pin, you can use jumper wires for convenience (use [Nucleo Board pinmap][1])
2. After connecting the additional LED, connect the board to the computer using a mini USB cable. Immediately after connecting, a console window should appear in Mbed studio.
3. Make sure you see the active program and the connection to the board in the upper left corner of the screen. If so, you can load the program to the disc using the ``Play`` button.
4. Watch the bottom left corner of the screen, windows will appear there telling you about the progress of the process. When uploading is finished, the LD1 LED will be lit continuously (blinking green and red when uploading)
5. Now click the **USER** button while watching the LED.
6. Now click the **USER** button once again, what happend with LED diode?
7. After finishing unplug Nucleo board from computer.

After finishing task try to read through the code with the comments. Observe how the code structure looks like and how individual objects are defined. **If you have any questions, just ask them.**

### Second part
------------------
The second task will be carried out using an IR distance sensor, and the main goal will be its calibration.

1. Connect the IR distance sensor to the PC_2 pin and the ground to the corresponding pin (use [Nucleo Board pinmap][1])
2. Read the IR sensor manual, the calibration process is described there step by step. Perform calibrations according to the instruction

    > [IR sensor tutorial](../markdown/ir_sensor.md)
3. After finishing unplug Nucleo board from computer.
## Summary
In first workshop, participants were introduced to the Nucleo-F446RE microcontroller and a custom PES board, acquiring familiarity with the Mbed Studio environment for coding. The workshop included the construction of the ``main`` file, comprehension of its structure, and implementation of essential commands. Additionally, exploration of hardware utilization, particularly the IR diode, and calibration using Matlab software were conducted. This foundational workshop aimed to put first steps in using microcontroler, laying the groundwork for further exploration in robotics and mechatronics.<br>
Questions for own consideration:
<!-- TODO think about it, about those question -->
- THINK ABOUT IT



**Answer:**
[WS1](../solutions/main_ws1.txt)

<!-- Links -->
[1]: https://os.mbed.com/platforms/ST-Nucleo-F446RE/
