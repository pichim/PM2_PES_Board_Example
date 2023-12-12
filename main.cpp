#include "mbed.h"

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

// main runs as an own thread
int main()
{
    const int main_task_period_ms = 50;

    Timer main_task_timer;
    Timer timer;

    // led on nucleo board
    // create DigitalOut object to command user led
    DigitalOut user_led(LED1);
    DigitalOut led1(PB_8);
    DigitalOut led2(PB_9);


    // Groove Ultrasonic Ranger V2.0
    float us_distance_cm = 0.0f;    // define variable to store measurement
    // PinName pin, float scale, float offset, int time
    // create range finder object (ultra sonic distance sensor), 20 Hz parametrization <-> main_task_period_ms = 50 ms
    // default pulsetime is 10
    // RangeFinder us_sensor(PB_D3,   // pin
    //                       5782.0f, // scale
    //                       0.02f,   // offset
    //                       17500);  // time
    // RangeFinder us_sensor(PB_D3, 5782.0f, 0.02f,  7000); // create range finder object (ultra sonic distance sensor), 50 Hz parametrization <-> main_task_period_ms = 20 ms
    UltrasonicSensor us_sensor(PB_D3);

    main_task_timer.start();
    timer.start();

    // this loop will run forever
    while (true) {
        main_task_timer.reset();
        timer.reset();

        us_distance_cm = us_sensor.read_cm();
        const int time_mus = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count();
        printf("%d, %f\n", time_mus, us_distance_cm);

        // toggling the user led
        user_led = !user_led;

        // read timer and make the main thread sleep for the remaining time span (non blocking)
        const int main_task_elapsed_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(main_task_timer.elapsed_time()).count();
        thread_sleep_for(main_task_period_ms - main_task_elapsed_time_ms);
    }
}

    // // pio.write_us(1, pulsetime);
    // io.output();
    // io = val;
    // wait_us(time);
    // io = !val;
    // // float t = (float)pio.read_high_us(timeout);
    // runtime.reset();
    // runtime.start();
    // pulsetime.reset();
    // io.input();
    // while (io == 1) {
    //     if (std::chrono::duration_cast<std::chrono::microseconds>(runtime.elapsed_time()).count() > timeout)
    //         return -1;
    // }
    // while (io == 0) {
    //     if (std::chrono::duration_cast<std::chrono::microseconds>(runtime.elapsed_time()).count() > timeout)
    //         return -1;
    // }
    // pulsetime.start();
    // while (io == 1) {
    //     if (std::chrono::duration_cast<std::chrono::microseconds>(runtime.elapsed_time()).count() > timeout)
    //         return -1;
    // }
    // pulsetime.stop();
    // return std::chrono::duration_cast<std::chrono::microseconds>(pulsetime.elapsed_time()).count();

    // if (t == -1.0)
    // {
    //     return -1.0;
    // }
    // return 100.0f * (t / scale + offset);

// // read ultra sonic distance sensor
// // us_distance_cm = us_range_finder.read_cm();
// // us_distance_cm = ultrasonic_sensor.read();
// us_distance_cm = ultrasonic_sensor;
// time_mus = std::chrono::duration_cast<std::chrono::microseconds>(timer.elapsed_time()).count();

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