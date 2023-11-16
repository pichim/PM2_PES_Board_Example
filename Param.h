#ifndef PARAM_H_
#define PARAM_H_

#include <eigen/Dense.h>

#define SDCARD_THREAD_TS_MS 20
#define SDCARD_THREAD_PRIORITY osPriorityNormal
#define SDCARD_THREAD_SIZE 4096
#define SDCARD_PIN_ADDITIONAL_LED PB_9
#define SDCARD_DO_PRINTF false
#define SDCARD_BUFFER_SIZE 512

#define IMU_THREAD_TS_MS 20
#define IMU_THREAD_PRIORITY osPriorityNormal
#define IMU_THREAD_SIZE 4096
#define IMU_PIN_SDA PC_9
#define IMU_PIN_SCL PA_8
#define IMU_DO_PRINTF true
#define IMU_DO_USE_STATIC_ACC_CALIBRATION false  // if this is false then acc gets averaged at the beginning and printed to the console
#define IMU_DO_USE_STATIC_MAG_CALIBRATION false  // if this is false then no mag calibration gets applied, e.g. A_mag = I, b_mag = 0
#define IMU_THREAD_DO_USE_MAG_FOR_MAHONY_UPDATE false

class Data
{
public:
    Data() {
        initialise();
    };
    virtual ~Data() {};

    Eigen::Vector3f gyro, acc, mag;
    Eigen::Quaternionf quat;
    Eigen::Vector3f rpy;
    float tilt = 0.0f;

    uint32_t t = 0;

private:
    void initialise() {
        gyro.setZero();
        acc.setZero();
        mag.setZero();
        quat.setIdentity();
        rpy.setZero();
    };
};

namespace Param {
    namespace IMU {
        // % bessel
        // p = 2;         % pole at p rad/s
        // kp = 2 * p;
        // ki = kp^2 / 3;
        static const float kp = 2.0f * 2.0f;
        static const float ki = kp * kp / 3.0f;
        
        static const Eigen::Matrix3f A_mag = ( Eigen::Matrix3f() <<  0.9789826f,  0.0000000f,  0.0000000f,
                                                                     0.0489856f,  0.9758227f,  0.0000000f,
                                                                     0.1323026f, -0.0493706f,  1.0451946f ).finished();
        static const Eigen::Vector3f b_mag = ( Eigen::Vector3f() << -0.0845453f,  0.2045368f, -0.4798227f ).finished();
        static const Eigen::Vector3f b_acc = ( Eigen::Vector3f() <<  0.0970105f, -0.2439377f,  0.0000000f ).finished();
    }
}
#endif /* PARAM_H_ */