#include "mbed.h"

#include "pm2_drivers/PESBoardPinMap.h"
#include "pm2_drivers/DebounceIn.h"

bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(USER_BUTTON);  // create InterruptIn interface object to evaluate user button falling and
                                      // rising edge (no blocking code in ISR)
void user_button_pressed_fcn();       // custom functions which get executed when user
                                      // button gets pressed, definition below

// main runs as an own thread
int main()
{
    // attach button fall function to user button object, button has a pull-up resistor
    user_button.fall(&user_button_pressed_fcn);

    // while loop gets executed every main_task_period_ms milliseconds (simple
    // aproach to repeatedly execute main)
    const int main_task_period_ms = 50; // define main task period time in ms e.g. 50 ms
                                        // -> main task runs 20 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task every main_task_period_ms
    Timer timer;

    // led on nucleo board
    // create DigitalOut object to command user led
    DigitalOut user_led(USER_LED);

    // additional led's
    // create DigitalOut object to command extra led (you need to add an aditional
    // resistor, e.g. 220...500 Ohm) an led as an anode (+) and a cathode (-), the
    // cathode is connected to ground via a resistor
    DigitalOut led1(PB_8);
    //DigitalOut led2(PB_9);

    // HERE DEFINE OBJECTS (remember about giving comments and proper names)
    
    // start timer
    main_task_timer.start();
    timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        if (do_execute_main_task) {
            
            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;

        } else {
            if (do_reset_all_once) {
                do_reset_all_once = false;
                led1 = 0;
            }
        }

        // toggling the user led
        user_led = !user_led;

        //Printing command 

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

void user_button_pressed_fcn()
{
    // do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    if (do_execute_main_task)
        do_reset_all_once = true;
}
