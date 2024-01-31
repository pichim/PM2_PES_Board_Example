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

Arm Mbed Studio is an integrated development environment (IDE) designed for embedded systems. It offers a user-friendly interface with features such as a powerful code editor, compiler, and debugger, streamlining the process of coding for Arm-based microcontrollers. Mbed Studio supports Mbed OS and enables seamless development, compilation, and debugging of embedded applications in a single environment.

<center><img src="../images/mbed_studio.png" alt="mbed studio" width="600" /></center>

### MBed OS Version

During the course we are working with MBed OS version 6. It is important to keep this in mind when you are looking for documentation or examples on the internet. There is also many of examples for older versions of MBed OS, especially MBed OS 2.

<!-- TODO: Generally from here the md file needs some love / attention i think
           I don't know if importing, building and flashing should be documented here, up to you
           but I think it should be documented briefly somewhere -->

### Importing your first project

### Building

## Flashing

<!-- TODO: Write this sentence better -->
After installing MBed studio, you need to create the program you will use during the class. It should be imported from the repository that was forked.
* Open the **File** menu and select **Import Program...**
<!-- TODO: No SSH : - ) -->
* Paste the full HTTPS URL of the relevant web page and (optionally) edit the program name. HTTPS URL can be found under the following box on your repository that was previously forked:

<!-- TODO: image is not in the folder, probably not pushed yet -->
<center><img src="../images/https_github.png" alt="https github" width="400" /></center>

* Add a **Program name**, it is a good a practice to name programs with capital letters, so they can be distinguished from libaries (lowercase letters).
* Then click **Add program**, by default this will be made as active program.

To complete the setup you need to choose the **Target** by typing the Nucleo Board that will be used: NUCLEO-F446RE. After you have completed this step, you can specify the **Build profile**, as Develop or Release. Finally you can build/compile the program by clicking the **Build button**.

<!-- TODO: Explain what happens when you connect the Nucleo Board to your computer -> additional drive you see when you plug in the board -->

<!-- TODO: Explain the Play Button -->

<!-- TODO: Explain where the Project is stored on the local machine and where to find the *.hex File -->

<!-- TODO: Explain that you can also flash the *.hex file by drag and drop to the additional drive you see when the Nucleo is attached to the computer -->

<!-- TODO: Make this prettier -->
<center><img src="../images/building_mbed.png" alt="https github" width="400" /></center>
