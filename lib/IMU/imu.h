#ifndef PIBOT_IMU_H_
#define PIBOT_IMU_H_

class IMU
{
public:
    virtual bool init() = 0;
    virtual void get_data(float* imu_data) = 0;
};

#endif
