#pragma once

class IMU
{
public:
    virtual bool init() = 0;
    virtual void get_data(float* imu_data) = 0;
};
