#pragma once
#include "dataframe.h"

class MotorController;
class MuSensorMegapi;
class Servo;

class Car
{
public:
   static Car* get(){
      static Car car;
      return &car;
   }

   void init();
   
   void servo_rotate();

   void motor_rotate();

private:
   Car() {}
   void init_motor();
   void init_servo();
   void init_mu_sensor();

private:
   MotorController* motor[MOTOR_COUNT];

   MuSensorMegapi* musensor;

   Servo* servo[SERVO_COUNT];
   
};