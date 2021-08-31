#pragma once

#include "dataframe.h"

class Transport;
class Dataframe;

class Robot:public Notify{
public:
   static Robot* get(){
      static Robot robot;
      return &robot;
   }

   // 初始化
   void init();

   // 確認上位機下發的命令
   void check_command();

   // 做運動學計算與里程計算
   void do_kinmatics_and_calc_odom();

   // 周邊設備更新
   void update_peripheral_equipment();
private:
   Robot(){}
   //初始化
   void init_trans();
   void init_motor_ctrl();
   void init_peripheral_equipment();
   
private:
   // Notify中的處理，特定消息觸發
   void update(const MESSAGE_ID id, void* data);

private:

   // 用於通訊的接口(uart)
   Transport*  trans;

   // 用於通訊的通訊協議接口(自訂的通訊協議)
   Dataframe*  frame;
};
