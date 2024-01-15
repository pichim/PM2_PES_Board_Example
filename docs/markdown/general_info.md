# Course setup
This site guides you through the essential steps to begin the course, starting from creating accounts on required platforms to downloading the relevant software used in the classes.

## Accounts
It is recommended to you use private E-mail address to set up accounts on Mbed and on GitHub. To use Matlab it is necessary to use university account because of the licence that allows you to download software.

- GitHub: https://github.com/
- Mbed: https://os.mbed.com/
- Matlab: https://matlab.mathworks.com/

## Software

- Mbed Studio: https://os.mbed.com/studio/ Free IDE for Mbed OS application and library development
- PuTTY: https://www.putty.org/ SSH and telnet client
- Matlab: https://matlab.mathworks.com/  programming and numeric computing platform
(Matlab can be downloaded but for those that have limited space on their desktops it is also possible to use online version)

## GitHub
GitHub is a web-based platform for version control using Git, facilitating collaborative software development by providing tools for code hosting, tracking changes, and managing project workflows. It allows developers to work on projects simultaneously, merge their changes seamlessly, and track the project's history. GitHub is widely used in the software development community for code collaboration, issue tracking, and hosting open-source projects.

* To create a fork of PM2 PES Board Example repository you need to go to the repository Github: https://github.com/pichim/PM2_PES_Board_Example
* On the top right corner click **Fork**:


<center><img src="../images/fork.png" alt="Forking repo" width="250" /></center>

* By default, forks are named the same as their upstream repositories. Optionally, to further distinguish your fork, in the "Repository name" field, type a name.
* Click **Create fork**.

In this way, you have your own branch of the main repository, which you will use during classes.

## Arm Mbed
Arm Mbed is a comprehensive embedded systems development platform designed for creating Internet of Things (IoT) applications. It offers a range of tools, libraries, and operating systems to simplify and accelerate the development process for Arm-based microcontrollers. Mbed provides a unified and scalable environment, supporting diverse applications in the field of embedded systems and IoT.

### Mbed Studio
Arm Mbed Studio is an integrated development environment (IDE) designed for embedded systems development. It offers a user-friendly interface with features such as a powerful code editor, compiler, and debugger, streamlining the process of coding for Arm-based microcontrollers. Mbed Studio supports the Mbed OS and enables seamless development, compilation, and debugging of embedded applications in a single environment.
<center><img src="../images/mbed_studio.png" alt="mbed studio" width="250" /></center>

**During course we are working with MBed OS 6.**

After installing MBed studio, you need to create the program you will use during the class. It should be imported from the repository that was forked.
* Open the **File** menu and select **Import Program...**
* Paste the full HTTPS or SSH URL of the relevant web page and (optionally) edit the program name. HTTPS URL can be find under following box on your repository that was previously forked:
<center><img src="../images/https_github.png" alt="https github" width="250" /></center>

* Add a **Program name**, it is good practice to name programs with capital letters.
* Then click **Add program**, by default this will be made as active program.

To compleate setup you need to choose the **Target** by typing the Nucleo Board that will be used so: NUCLEO-F446RE. After that you can specyfi the **Build profile**, as Develop and at the end of the process you can click **Hammer button** that will build the program on your desktop.  
<center><img src="../images/building_mbed.png" alt="https github" width="250" /></center>
