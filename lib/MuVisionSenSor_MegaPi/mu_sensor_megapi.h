#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <MuVisionSensor.h>
#include <SimpleKalmanFilter.h>

#define X 1
#define Y 2
#define Width 3
#define High 4

class MuSensorMegapi{
   public:
      MuSensorMegapi(int32_t mu_address);
      void init(uint16_t version);
      void init(uint16_t version,uint8_t color);
      bool Have_someone_in_front_of_camera();
      int GetValue(short object_inf);
   private:
      uint16_t version;
      uint8_t color;
      uint8_t err = 0;
      float xAxis,yAxis;
      float WidthValue,HeightValue;
   private:
      MuVisionSensor MU;
      SimpleKalmanFilter   kalman_servo1;
      SimpleKalmanFilter   kalman_servo2;
};