#pragma once

class IMU;
class Joystick;

class Peripheral_equipment{
public:
   static Peripheral_equipment* get(){
      static Peripheral_equipment peripheral_equipment;
      return &peripheral_equipment;
   }

   void init();

   void get_imu_data();

   void check_joystick();

private:
   Peripheral_equipment(){}
   void init_imu();
   void init_joystick();

private:
   IMU*  imu;

   Joystick*   joystick;
};