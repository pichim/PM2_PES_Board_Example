# Main File Description

In general a file is executed starting from top to buttom and the main file is the first file that is executed when the microcontroller is powered on.

The main file is the file that contains the ``main()`` function, which is the function that is executed by the microcontroller.

At the very beginning of the file, the mbed library is included, which is a library that contains all the necessary functions and objects for the microcontroller to work properly with the mbed operating system.

```
#include "mbed.h"
```

Next a map / dictionary of the PES board is included. In here we specify a naming conventaion to translate pin names from the PES board to the Nucleo board, see also [pes_board_peripherals.pdf](docs/datasheets/pes_board_peripherals.pdf).

```
// pes board pin map
#include "pm2_drivers/PESBoardPinMap.h"
```

Next we define additional drivers for hardware that we use in the project, for now we only include the ``DebounceIn.h`` driver, which is the driver we use to read in the blue **USER** button on the Nucleo board.

```
// drivers
#include "pm2_drivers/DebounceIn.h"
```

Beginning now until the beginning of the ``main()`` function is space to define global variables and functions. These are placed outside of any scope, e.g. the scope of the ``main(){scope from main}`` function and are there for known by any scope within the ***main.cpp*** file (can be accessed from every scope).

Be aware that it is not possible to execute any function globally. Functions can only be executed from within a scope, e.g. the scope of the ``main(){scope of main}`` function.

```
bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(USER_BUTTON); // create DebounceIn object to evaluate the user button
                                     // falling and rising edge
void toggle_do_execute_main_fcn();   // custom function which is getting executed when user
                                     // button gets pressed, definition below
```

``toggle_do_execute_main_fcn()`` is declared here, but defined below the ``main()`` function. This is the function that will be executed when the blue **USER** button is pressed. It is declared here, so that the ``main()`` function knows that it exists and can be executed. The actual definition of the function is placed below the ``main()`` function at the end of the file.

Below the code snippet of the first part of the ``main()`` function. All objects and variables that are defined here are visible only from the scope of ``main(){scope of main}``.

Be aware that code always should be as local as possible. So when ever possible, objects and variables should be defined within the scope of the part where they are actually used. Working with global variables and objects can lead to confusion and errors, especially when the project grows and becomes more complex (prone to errors).

```
// main runs as an own thread
int main()
{
    // attach button fall function address to user button object, button has a pull-up resistor
    user_button.fall(&toggle_do_execute_main_fcn);

    // while loop gets executed every main_task_period_ms milliseconds, this is a
    // simple approach to repeatedly execute main
    const int main_task_period_ms = 20; // define main task period time in ms e.g. 20 ms, there for
                                        // the main task will run 50 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task
                                        // every main_task_period_ms

    // led on nucleo board
    DigitalOut user_led(USER_LED);

    // additional led
    // create DigitalOut object to command extra led, you need to add an aditional resistor, e.g. 220...500 Ohm
    // a led has an anode (+) and a cathode (-), the cathode needs to be connected to ground via a resistor
    DigitalOut led1(PB_9);

    // start timer
    main_task_timer.start();
```

Above the place in the ``main()`` function where you should define additional objects and variables. Here you can also execute functions, because now we are operating within a scope, e.g. ``user_button.fall(&toggle_do_execute_main_fcn)``.

Below starts the ``while()`` loop, since the argument is ``while(true)`` and we don't have a break condition the program will repeat everything in the scope of ``while(){scope of while}`` until the end of time (or reset, or reflash... :-)).

If we want to command an actuator, read in a sensor or print output to the console or generally; perform any operations in an infinite manner, the corresponding task needs to be implemented within the ``while()`` loop.


```
    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        if (do_execute_main_task) {
```

Everything in the ``if()`` statement will be executed if the blue **USER** button is pressed, which is toggling the state of ``do_execute_main_task`` via the ``toggle_do_execute_main_fcn()`` function. Here you can insert commands that you want to execute only after pressing the button.

```
            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;
        } else {
            // the following code block gets executed only once
            if (do_reset_all_once) {
                do_reset_all_once = false;
```

Here we can insert commands that we want to execute only after pressing the blue **USER** button for the second time and this code is only executed once. For now, we reset the state of the ``led1`` object to 0, which turns off the LED.

```
                // reset variables and objects
                led1 = 0;
            }
        }

        // toggling the user led
        user_led = !user_led;
```
Here we perform a time measurement and make the main thread sleep for the remaining time span (non blocking). This is a simple approach to repeatedly execute the main task every ``main_task_period_ms`` milliseconds. The ``main_task_period_ms`` is defined at the beginning of the ``main()`` function and is set to 20 ms. This means that the main task will run 50 times per second.

```
        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}
```

At the end of the file, the ``toggle_do_execute_main_fcn()`` function is defined. We declared it at the top of the file but define it here.

This function is executed when the blue **USER** button is pressed. It toggles the state of the ``do_execute_main_task`` variable and sets the ``do_reset_all_once`` variable to true if the ``do_execute_main_task`` variable changed from false to true.

```
void toggle_do_execute_main_fcn()
{
    // toggle do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    // set do_reset_all_once to true if do_execute_main_task changed from false to true
    if (do_execute_main_task)
        do_reset_all_once = true;
}
```