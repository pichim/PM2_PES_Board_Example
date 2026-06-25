#include "mbed.h"

// pes board pin map
#include "PESBoardPinMap.h"

// drivers
#include "DebounceIn.h"

// IR sensor
#include "IRSensor.h"

float ir_distance_mv = 0.0f;
float ir_distance_cm = 0.0f;
AnalogIn ir_analog_in(PC_2); // create AnalogIn object for IR sensor, pin is defined in PESBoardPinMap.h
const float cal_a = 9227.1320f; // calibration value a for IR sensor, you need to insert the value that you got from the MATLAB file
const float cal_b = -350.8126f; // calibration value b for IR sensor
IRSensor ir_sensor(PC_2, cal_a, cal_b); // create IRSensor object for IR sensor, pin is defined in PESBoardPinMap.h

bool do_execute_main_task = false; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(BUTTON1);   // create DebounceIn to evaluate the user button
void toggle_do_execute_main_fcn(); // custom function which is getting executed when user
                                   // button gets pressed, definition at the end

float ir_sensor_compensation(float ir_distance_mv);

// main runs as an own thread
int main()
{
    // attach button fall function address to user button object
    user_button.fall(&toggle_do_execute_main_fcn);

    // while loop gets executed every main_task_period_ms milliseconds, this is a
    // simple approach to repeatedly execute main
    const int main_task_period_ms = 1000; // define main task period time in ms e.g. 20 ms, therefore
                                        // the main task will run 50 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task
                                        // every main_task_period_ms

    // led on nucleo board
    DigitalOut user_led(LED1);

    // additional led
    // create DigitalOut object to command extra led, you need to add an additional resistor, e.g. 220...500 Ohm
    // a led has an anode (+) and a cathode (-), the cathode needs to be connected to ground via the resistor
    DigitalOut led1(PB_9);

    // --- adding variables and objects and applying functions starts here ---

    // start timer
    main_task_timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();

        // --- code that runs every cycle at the start goes here ---

        if (do_execute_main_task) {

            // --- code that runs when the blue button was pressed goes here ---

            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;
        } else {
            // the following code block gets executed only once
            if (do_reset_all_once) {
                do_reset_all_once = false;

                // --- variables and objects that should be reset go here ---

                // reset variables and objects
                led1 = 0;
                ir_distance_mv = 0.0f;
            }
        }

        // toggling the user led
        user_led = !user_led;

        // --- code that runs every cycle at the end goes here ---

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        int main_task_elapsed_time_ms = duration_cast<milliseconds>(main_task_timer.elapsed_time()).count();
        if (main_task_period_ms - main_task_elapsed_time_ms < 0)
            printf("Warning: Main task took longer than main_task_period_ms\n");
        else
            thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);

        ir_distance_mv = ir_analog_in.read() * 3.3f * 1000.0f; // read voltage in mV
        ir_distance_cm = ir_sensor_compensation(ir_distance_mv); // apply compensation
        // ir_distance_cm = ir_sensor.readcm(); // read distance in cm from IRSensor object
        // print to the serial terminal
        printf("IR distance mV: %f IR distance cm: %f \n", ir_distance_mv, ir_distance_cm);
    }
}

void toggle_do_execute_main_fcn()
{
    // toggle do_execute_main_task if the button was pressed
    do_execute_main_task = !do_execute_main_task;
    // set do_reset_all_once to true if do_execute_main_task changed from false to true
    if (do_execute_main_task)
        do_reset_all_once = true;
}

float ir_sensor_compensation(float ir_distance_mv)
{
    // insert values that you got from the MATLAB file
    static const float a = 9227.1320f;
    static const float b = -350.8126f;

    // avoid division by zero by adding a small value to the denominator
    if (ir_distance_mv + b == 0.0f)
        ir_distance_mv -= 0.001f;

    return a / (ir_distance_mv + b);
}