# Main file description

Here is the place where the mbed library is included.
```
#include "mbed.h"
```
Here a map of the PES board is included inside which is a map of the board pins referenced to the pins on the Nucleo board.
```
// pes board pin map
#include "pm2_drivers/PESBoardPinMap.h"
```
Here is the place to attach the appropriate drivers, depending on what hardware we use in the project.
```
// drivers
#include "pm2_drivers/DebounceIn.h"
```
The following zone is the zone for defining variables and global functions. Whenever a variable or function or object is used outside the ```main()``` function, it should be defined here. In addition, whenever an additional function outside ```main()``` is defined, its actual definition may be at the end of the file, however, it must be defined here, example: ```void toggle_do_execute_main_fcn();```

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
Below is the ```main()``` function, which is the main function performed by the microcontroller. All objects and variables that are defined inside are visible only to this function, you can't see them from the global point of view.
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
Above is the place in the ``main()`` function where you should define individual objects or variables, note that here they are defined only once, at the start of the program. Whenever we need to define a new object or variable, which in a later part will be filled with actual values this place is used for that.

Below starts the ``while(true)`` function, so specified it will execute the loop an infinite number of times. If we want to log data, print output to the console or generally perform operations in an infinite manner, the appropriate definitions should be placed inside this loop.
```
    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        if (do_execute_main_task) {
```
Everything in the ``if()`` statement will be enforced if the blue **USER** button is pressed. Here you should insert those commands that you want to execute only after making such a decision, i.e. pressing the button. 
```
            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;
        } else {
            // the following code block gets executed only once
            if (do_reset_all_once) {
                do_reset_all_once = false;

                // reset variables and objects
                led1 = 0;
            }
        }

        // toggling the user led
        user_led = !user_led;

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}
```
Below is the external function whose declaration was already placed above, before the ``main()`` function.
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