#include "GY65.h"

#define ADX_SCALE 0.00239420166015625  // 4mg/LSB    16/63356*9.80665
#define GRY_SCALE 0.001065264417860243  //rad/s   4000/65536*(PI/180)

#define WRITE_INTERVAL 50000

bool GY65::init(){
   Wire.begin();

   if(!mpu6050.testConnection()){
      return false;
   }

   mpu6050.initialize();
   return true;

}

void GY65::get_data(float imu_data[9]){
   mpu6050.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

   imu_data[0] = ax*ADX_SCALE;
   imu_data[1] = ay*ADX_SCALE;
   imu_data[2] = az*ADX_SCALE;
   imu_data[3] = gx*GRY_SCALE;
   imu_data[4] = gy*GRY_SCALE;
   imu_data[5] = gz*GRY_SCALE;
}