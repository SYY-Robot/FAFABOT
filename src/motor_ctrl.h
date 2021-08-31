#pragma once

#if ROBOT_MODEL == ROBOT_MODEL_2WD_DIFF
#include "differential.h"
#endif

#if ROBOT_MODEL == ROBOT_MODEL_4WD_DIFF
#include "differential4wd.h"
#endif

#if ROBOT_MODEL == ROBOT_MODEL_3WD_OMNI
#include "omni3.h"
#endif

#if ROBOT_MODEL == ROBOT_MODEL_4WD_MECANUM
#include "mecanum.h"
#endif

class MotorController;
class Encoder;
class PID;
class Model;

class Motor_ctrl{
public:
   static Motor_ctrl* get(){
      static Motor_ctrl motor_ctrl;
      return &motor_ctrl;
   }

   // 初始化
   void init();

   // 清除里程
   void clear_odom();
   
   // 更新機器人當前速度
   void update_velocity();

   // 運動解算
   void do_kinmatics();

   // 計算里程
   void calc_odom();

   // 更新PID參數
   void update_pid();

   // 更新運動模型參數
   void update_model();


private:
   Motor_ctrl(){}


private:
   MotorController*  motor[MOTOR_COUNT];

   Encoder*          encoder[MOTOR_COUNT];

   PID*              pid[MOTOR_COUNT];

   float             input[MOTOR_COUNT];

   float             feedback[MOTOR_COUNT];

   Model*            model;

   Odom              odom;

private:
   bool do_kinmatics_flag;

   unsigned long last_velocity_command_time;
};