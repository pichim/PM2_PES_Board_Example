# Course setup

This site guides you through the essential steps to begin the course, starting from creating accounts on required platforms to downloading the relevant software used during the workshops and for your project.

## Accounts

It is recommended that you use a private E-mail address to set up accounts on Mbed and on GitHub. To be able to use Matlab it is mandatory to use the account from your university because of the corresponding license.

- GitHub: https://github.com/ Private E-mail
- Mbed: https://os.mbed.com/ Private E-mail
- Matlab: https://matlab.mathworks.com/ University account

## Software

- Mbed Studio: https://os.mbed.com/studio/ Free IDE for the Mbed OS ecosystem
- PuTTY: https://www.putty.org/ SSH and telnet client
- Matlab: https://matlab.mathworks.com/  Programming and nummerical computing platform

## GitHub

GitHub is a web-based platform from Microsoft for version control using Git, facilitating collaborative software development by providing tools for code hosting, tracking changes, and managing project workflows. It allows developers to work on projects simultaneously, merge their changes seamlessly, and track the project's history. GitHub is widely used in the software development community for code collaboration, issue tracking, and hosting open-source projects.

* To create a fork of PM2 PES Board Example repository you need to go to the repository Github: https://github.com/pichim/PM2_PES_Board_Example
* On the top right corner click **Fork**:

<center><img src="../images/fork.png" alt="Forking repo" width="600" /></center>

* By default, forks are named the same as their upstream repositories. Optionally, to further distinguish your fork, in the "Repository name" field, type a different name.
* Click **Create fork**.

By forking the repository, you can freely experiment with changes without affecting the original project. This fork is now your own version of the repository. You can make changes to files and push commits to your fork's branch.

## Arm Mbed

Arm Mbed is a comprehensive embedded systems development platform designed for embedded systems. It offers a range of tools, libraries, and acces to a real time C++ based OS to simplify and accelerate the development process for Arm-based microcontrollers. Mbed provides a unified and scalable environment, supporting diverse applications in the field of embedded systems and IoT.

### Mbed Studio
-------------------------
Arm Mbed Studio is an integrated development environment (IDE) designed for embedded systems. It offers a user-friendly interface with features such as a powerful code editor, compiler, and debugger, streamlining the process of coding for Arm-based microcontrollers. Mbed Studio supports Mbed OS and enables seamless development, compilation, and debugging of embedded applications in a single environment.

<center><img src="../images/mbed_studio.png" alt="mbed studio" width="600" /></center>

### MBed OS Version
-------------------------
During the course we are working with MBed OS version 6. It is important to keep this in mind when you are looking for documentation or examples on the internet. There is also many of examples for older versions of MBed OS, especially MBed OS 2.

### Importing your first project
-------------------------
Importing a program from GitHub to Mbed Studio involves linking the local repository with Mbed Studio, enabling seamless collaboration and development on the chosen project:
* Once MBed Studio is installed, the next step involves creating the program intended for use during the class. This program should be imported from the previously forked repository. Open the **File** menu and select **Import Program...**
<center><img src="../images/importing_mbed.png" alt="Importing program" width="400" /></center>

* Visit the GitHub page of your repository (e.g., https://github.com/YOUR_NICK/NAME_OF_FORKED_REPOSITORY) and copy the **HTTPS** URL, located below the box on your forked repository.
<center><img src="../images/https_github.png" alt="https github" width="400" /></center>

* Paste the full HTTPS URL of the relevant web page and (optionally) edit the program name. * If you are changing **Program name**, it is a good a practice to name programs with capital letters, so they can be distinguished from libaries (lowercase letters).
<center><img src="../images/importing_mbed_1.png" alt="Importing program 1" width="400" /></center>

* Then click **Add program**, by default this will be made as active program.
* To complete the setup you need to choose the **Target** by typing the Nucleo Board that will be used: NUCLEO-F446RE.
<center><img src="../images/importing_mbed_2.png" alt="Importing program 2" width="400" /></center>

### Building
-------------------------
Building a program in Mbed Studio involves the process of compiling and linking the source code to create an executable binary file (*.hex file) that can run on a specific microcontroller or embedded system. The build process ensures that the code is translated into machine code compatible with the target platform, and it may involve resolving dependencies, optimizing code, and generating the necessary files for deployment. Once the program is successfully built, the resulting binary file can be loaded onto the microcontroller board to execute the desired functionality. Building is a crucial step to convert human-readable code into a format understandable by the hardware.

In Mbed Studio, there are three build profiles:

1. **Debug Profile:**
   - **Purpose:** Primarily used for debugging and development.
   - **Optimizations:** Limited optimizations to aid debugging, resulting in larger binaries.
   - **Symbols:** Includes debugging symbols to facilitate source-level debugging.
   - **Compile Time:** Longer compile times due to additional information included.

2. **Develop Profile:**
   - **Purpose:** Balanced profile for development and testing.
   - **Optimizations:** Moderate level of optimizations to balance size and performance.
   - **Symbols:** Includes debugging symbols for effective debugging.
   - **Compile Time:** Moderate compile times.

3. **Release Profile:**
   - **Purpose:** Optimized for production or release.
   - **Optimizations:** High level of optimizations for smaller and faster binaries.
   - **Symbols:** Debugging symbols are excluded, reducing binary size.
   - **Compile Time:** Faster compile times compared to debug profiles.

Choosing the appropriate build profile depends on the development stage and requirements. Debug profiles aid in effective debugging, Develop profiles offer a balanced compromise, while Release profiles optimize for size and performance in production environments. <br>
The build process, which translates human-readable code into a format understandable by the hardware, can be performed without a connected board. After importing the program, specify build profile and initiate the build process by clicking the **HAMMER** button.
<center><img src="../images/building_mbed.png" alt="Building program" width="400" /></center>

After the building process in Mbed Studio, the compiled files, including *.hex file are typically stored in the **BUILD** directory within your project folder. 

### Flashing
-------------------------
Flashing a microcontroller board involves programming its non-volatile memory with the compiled binary of your program, enabling it to execute the code during startup. After building your program and connecting the board to your computer, click the **PLAY** button in Mbed Studio to initiate the flashing process. This transfers the compiled binary to the microcontroller's memory, making it ready to run the programmed code.
<center><img src="../images/flash_mbed.png" alt="Flashing board" width="400" /></center>

To accommodate code changes done during developing, you can simply click the **PLAY** button again, prompting Mbed Studio to update the translated file with new elements before transferring it to the board.

**NOTE:** Periodically deleting the build folder and re-building the program is recommended to avoid potential interference that may arise from adding new code.

## Navigating the Environment

### Connected Board
-------------------------
Upon connecting the Nucleo Board to your computer, it functions as an additional drive in the file explorer. This drive represents the built-in mass storage feature facilitated by the Nucleo Board's on-board ST-Link programmer/debugger. Detected as a removable drive, it often adopts a name like "NODE_F446RE" based on the specific Nucleo model.

This drive serves as a convenient avenue for transferring the compiled binary (firmware) of your program to the Nucleo Board. You can easily drag and drop the compiled binary file onto this drive, and the ST-Link interface will subsequently flash the microcontroller's memory with the updated firmware. This process is integral to the flashing or programming step, ensuring your microcontroller is equipped with the latest code.

### File Storage Location
-------------------------
In Mbed Studio, the Mbed project folder is typically situated in the workspace directory where you created or imported the project. To locate the Mbed project folder and the .hex file:

**1. Project Folder Location:**
- Open the "This PC" folder.
- Navigate to the "Windows (C:)" drive.
- Go to the "Users" directory.
- Find the directory corresponding to your profile where Mbed Studio is installed.
- Look for the "Mbed Programs" directory within your profile directory.
- Inside "Mbed Programs," you'll find folders containing your Mbed programs.

**2. Locating the .hex File:**
- Inside your project folder, go to the "BUILD" directory.
- Within the "BUILD" directory, locate the folder named after your target hardware (e.g., "NUCLEO_F446RE").
- Open this specific folder, where you will find the .hex file bearing the project name and ".hex" extension.

This .hex file serves as the compiled binary for flashing your microcontroller with the updated firmware. You can either drag and drop it onto the corresponding drive representing your microcontroller or utilize external tools for programming.
