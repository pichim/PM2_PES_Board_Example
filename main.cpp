#include "mbed.h"

#include "pm2_drivers/DebounceIn.h"
#include "pm2_drivers/UltrasonicSensor.h"
#include "pm2_drivers/RangeFinder/RangeFinder.h"
#include "pm2_drivers/RangeFinder/Pulse.h"

// PES-Board Pin Names
#define PB_D0 PB_2
#define PB_D1 PC_8
#define PB_D2 PC_6
#define PB_D3 PB_12

#define PB_M1_PWM PB_13
#define PB_M2_PWM PA_9
#define PB_M3_PWM PA_10

#define PB_M1_ENC_A PA_6
#define PB_M1_ENC_B PC_7
#define PB_M2_ENC_A PB_6
#define PB_M2_ENC_B PB_7
#define PB_M3_ENC_A PA_0
#define PB_M3_ENC_B PA_1

// #include "pm2_drivers/PESBoardPinName.h"

bool do_execute_main_task = true; // this variable will be toggled via the user button (blue button) and
                                   // decides whether to execute the main task or not
bool do_reset_all_once = false;    // this variable is used to reset certain variables and objects and
                                   // shows how you can run a code segment only once

// objects for user button (blue button) handling on nucleo board
DebounceIn user_button(PC_13);  // create InterruptIn interface object to evaluate user button falling and
                                // rising edge (no blocking code in ISR)
void user_button_pressed_fcn(); // custom functions which get executed when user
                                // button gets pressed, definition below

// PinName pin = PB_D3;
// float scale = 5782.0f;
// float offset = 0.02f;
// int timeout = 17500;
// int pulsetime = 10;
// // PulseInOut pio(pin);
// float t{0.f};
// DigitalInOut io(pin);
// Timer pulsetimer, runtimer;
// // Timeout m_Timeout;
// InterruptIn m_InteruptIn(pin);

// void interupt_rise(){
//     pulsetimer.reset();
//     pulsetimer.start();
// }

// void interupt_fall(){
//     pulsetimer.stop();
//     t =  (float)std::chrono::duration_cast<std::chrono::microseconds>(pulsetimer.elapsed_time()).count();
// }

// void enableDigitalOutput()
// {
//     io = 1;
// }
// void disableDigitalOutput()
// {
//     io = 0;
// }

// main runs as an own thread
int main()
{
    enum State {
        INIT,
        MEASURE,
    } state = State::INIT;

    // attach button fall function to user button object, button has a pull-up resistor
    user_button.fall(&user_button_pressed_fcn);

    // while loop gets executed every main_task_period_ms milliseconds (simple
    // aproach to repeatedly execute main)
    const int main_task_period_ms = 20; // define main task period time in ms e.g. 50 ms
                                        // -> main task runs 20 times per second
    Timer main_task_timer;              // create Timer object which we use to run the main task every main_task_period_ms
    Timer timer;

    // led on nucleo board
    // create DigitalOut object to command user led
    DigitalOut user_led(LED1);

    // additional led's
    // create DigitalOut object to command extra led (you need to add an aditional
    // resistor, e.g. 220...500 Ohm) an led as an anode (+) and a cathode (-), the
    // cathode is connected to ground via a resistor
    DigitalOut led1(PB_8);
    DigitalOut led2(PB_9);

    // mechanical button
    DigitalIn mechanical_button(PC_5); // create DigitalIn object to evaluate extra mechanical button, you
                                       // need to specify the mode for proper usage, see below
    mechanical_button.mode(PullUp);    // set pullup mode: sets pullup between pin and 3.3 V, so that there
                                       // is a defined potential

    // Groove Ultrasonic Ranger V2.0
    float us_distance_cm = 0.0f;    // define variable to store measurement
    // PinName pin, float scale, float offset, int time
    // create range finder object (ultra sonic distance sensor), 20 Hz parametrization <-> main_task_period_ms = 50 ms
    // default pulsetime is 10
    // RangeFinder us_range_finder(PB_D3,   // pin
    //                             5782.0f, // scale
    //                             0.02f,   // offset
    //                             17500);  // time
    // RangeFinder us_range_finder(PB_D3, 5782.0f, 0.02f,  7000); // create range finder object (ultra sonic distance sensor), 50 Hz parametrization <-> main_task_period_ms = 20 ms
    UltrasonicSensor ultrasonic_sensor(PB_D3);

    main_task_timer.start();
    timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();
        timer.reset();
        int time_mus = 0;

        if (do_execute_main_task) {
            // visual feedback that the main task is executed, setting this once would actually be enough
            led1 = 1;
            
            // state machine
            switch (state) {
                case State::INIT:
                    if (!mechanical_button.read())
                        state = State::MEASURE;

                    break;

                case State::MEASURE:
                    led2 = 1;

                    // read ultra sonic distance sensor
                    // us_distance_cm = us_range_finder.read_cm();
                    // us_distance_cm = ultrasonic_sensor.read();
                    us_distance_cm = ultrasonic_sensor;
                    time_mus = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count();

                    // // pio.write_us(1, pulsetime);
                    // // timer.reset(); //  measurement: 9889
                    // m_InteruptIn.fall(NULL);
                    // m_InteruptIn.rise(NULL);
                    // io.output();
                    // enableDigitalOutput();
                    // wait_us(pulsetime);
                    // disableDigitalOutput();
                    // // enableDigitalOutput();
                    // // m_Timeout.attach(&disableDigitalOutput, std::chrono::microseconds{pulsetime});
                    // // t = (float)pio.read_high_us(timeout);
                    // runtimer.reset();
                    // runtimer.start();

                    // // timer.reset(); //  measurement: 9871
                    // io.input();
                    // m_InteruptIn.fall(&interupt_fall);
                    // m_InteruptIn.rise(&interupt_rise);

                    // // while (io == 1) {
                    // //     if (std::chrono::duration_cast<std::chrono::microseconds>(runtimer.elapsed_time()).count() > timeout)
                    // //         t = -1;
                    // // }
                    // // // timer.reset(); //  measurement: 9871
                    // // while (io == 0) {
                    // //     if (std::chrono::duration_cast<std::chrono::microseconds>(runtimer.elapsed_time()).count() > timeout)
                    // //         t =  -1;
                    // // }
                    // // // timer.reset(); //  measurement: 9388
                    // // pulsetimer.reset();
                    // // pulsetimer.start();
                    // // while (io == 1) {
                    // //     if (std::chrono::duration_cast<std::chrono::microseconds>(runtimer.elapsed_time()).count() > timeout)
                    // //         t =  -1;
                    // // }
                    // // pulsetimer.stop();
                    // // // timer.reset(); //  measurement: 4
                    // // t =  (float)std::chrono::duration_cast<std::chrono::microseconds>(pulsetimer.elapsed_time()).count();

                    // if (t == -1.0)
                    // {
                    //     us_distance_cm =   -1.0;
                    // } else {
                    //     us_distance_cm =  100.0f * (t / scale + offset);
                    // }
                    // time_mus = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count();
                    

                    break;

                default:
                    break; // do nothing
                }
        } else {
            if (do_reset_all_once) {
                // toggle do_reset_all_once to make sure this code is only executed once
                do_reset_all_once = false;
                us_distance_cm = 0.0f;
                led1 = led2 = 0;
                state = State::INIT;
            }
        }

        printf("%d, %f\n", time_mus, us_distance_cm);

        // toggling the user led
        user_led = !user_led;

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