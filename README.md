# Directory
dirHome = getwd()

# Image directory
dirImg = paste(dirHome, '/screenshots/image.jpeg', sep = "")

# PM2_PES_Board

Maciej Szarek\
szar@zhaw.ch\
TE 302

Michael Peter\
pmic@zhaw.ch\
TE 307

# Table of Contents
1. [Course setup](docs/markdown/general_info.md)
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
3. [Tips](#tips)
4. [Workshops](#workshops)
    * [Workshop 1](docs/markdown/ws1.md)
    * [Workshop 2](docs/markdown/ws2.md)
    * [Workshop 3](docs/markdown/ws3.md)


<!-- # Using the PES-Board with power On and hardware running while connected to your computer happens at your own risk

# Under now circumstances leave the battery charger connected to the PES-Board when using the PES-Board of having it connected to your computer

# Motor encoder soldering can be wrong. Do not assume that if you plug in one Motor and everything works you can the also use the same pings and everything works. You have to make sure that the physical rotation is according to your assumption and that a positive input leads to a positive change of increments.

# Depending on the PES BOARD version DCMotor M3 rotation direction might be inverted

# Depending on the PES BOARD version the Pin map might differ

- FastPWM default pwm period is 50 mus

-->
## Course setup
In order to be able to complete the course, one must first register on the platforms and obtain the appropriate tools. All the information needed to start the course can be found at the link: [Course setup](docs/markdown/general_info.md)

## Hardware
In our course, we'll be utilizing the Nucleo-F446RE microcontroller, which is paired with the PES board designed and developed at ZHAW by ... . The setup includes a variety of sensors and actuators for practical applications.
### Nucleo F446RE
------------------------
The Nucleo-F446RE is a microcontroller development board featuring the STM32F446RET6 microcontroller from STMicroelectronics. It provides a versatile platform for prototyping and developing embedded systems, offering a wide range of peripherals, connectivity options, and compatibility with the Arduino and ST Morpho ecosystems. All of the technical details can be found here: https://www.st.com/en/microcontrollers-microprocessors/stm32f446re.html#documentation 

<center><img src="docs/images/nucleo_overview.png" alt="Nucleo overview" width="600" /></center>

Arm MBed has prepared a special platform containing key information on the controller used. On it you can find technical information, but most importantly the pinmap of outputs, which is necessary for the correct connection of peripheral devices.
#### Nucleo platform (pinmap)
Link: https://os.mbed.com/platforms/ST-Nucleo-F446RE/

### PES Board
------------------------
PES board is a board created to work with a microcontroller on which appropriate outputs and inputs are designed to allow a more orderly connection of peripheral devices. It provides multiple pinouts for:
- 3 DC-Motor (brushed)
- 4 Servos (occupy the 4 DI/O if used)
- 4 DI/O, 3.3V (5V tolerant)
- 4 AI/O, 3.3V (5V tolerant)
- 3 Encoder-Counter

><b>Important information related to working with PES Board:</b>
>- <b>Using the PES-Board with power On and hardware running while connected to your computer happens at your own risk</b>
>- <b>Under now circumstances leave the battery charger connected to the PES-Board when using the PES-Board of having it connected to your computer </b>
>- <b>Motor encoder soldering can be wrong. Do not assume that if you plug in one Motor and everything works you can the also use the same pings and everything works. You have to make sure that the physical rotation is according to your assumption and that a positive input leads to a positive change of increments</b>
>- <b>Depending on the PES BOARD version DCMotor M3 rotation direction might be inverted</b>
>- <b>Depending on the PES BOARD version the Pin map might differ</b>

The kits come with battery packs that power the motors. Each Battery Pack provides 6V, allowing the motors to function with a 6V power supply when one set is connected or 12V when both are connected. It's crucial to bridge the pins on the second socket if you intend to use the 6V kit.

#### Pheripherals
<!-- [pes_board_peripherals.PNG](docs/images/pes_board_peripherals.PNG "PES Board Pheripherals") -->

<center><img src="docs/images/pes_board_peripherals.png" alt="Nucleo overview" width="600" /></center>

PDF: [pes_board_peripherals.pdf](docs/datasheets/pes_board_peripherals.pdf)

#### Pin-Mapping
PDF: [pes_board_pin_mapping.pdf](docs/datasheets/pes_board_pin_mapping.pdf)

### Hardware Tutorials
------------------------
The following links contain the hardware tutorials:
- [<font size="3"> IR Sensor </font>](docs/markdown/ir_sensor.md)
- [<font size="3"> Ultrasonic Sensor </font>](docs/markdown/ultrasonic_sensor.md)
- [<font size="3"> Servo </font>](docs/markdown/servo.md)
- [<font size="3"> DC Motor </font>](docs/markdown/dc_motor.md)

## Tips

## Workshops
The following links contain the workshops instructions:
- [<font size="3"> Workshop 1 </font>](docs/markdown/ws1.md)
- [<font size="3"> Workshop 2 </font>](docs/markdown/ws2.md)
- [<font size="3"> Workshop 3 </font>](docs/markdown/ws3.md)

## Links