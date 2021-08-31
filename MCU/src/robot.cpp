#include "Arduino.h"
#include "robot.h"

#include "board.h"
#include "usart_transport.h"
#include "simple_dataframe_slave.h"
#include "data_holder.h"

#include "motor_ctrl.h"
#include "peripheral_equipment.h"

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
void Robot::init(){
   Data_holder::get()->load_parameter();

   init_trans();

   init_motor_ctrl();

   init_peripheral_equipment();
}

void Robot::init_trans(){
   static USART_transport _trans(MASTER_USART,115200);

   static Simple_dataframe _frame(&_trans);

   trans = &_trans;
   frame = &_frame;

   trans->init();
   frame->init();

   frame->register_notify(ID_SET_ROBOT_PARAMTER,this);
   frame->register_notify(ID_CLEAR_ODOM, this);
   frame->register_notify(ID_SET_VELOCITY, this);
   
}

void Robot::init_motor_ctrl(){
   Motor_ctrl::get()->init();
}

void Robot::init_peripheral_equipment(){
   Peripheral_equipment::get()->init();
}

void Robot::check_command(){
   unsigned char ch=0;
   if(trans->read(ch)){
      if(frame->data_recv(ch)){
         frame->data_parse();
      }
   }
}

void Robot::update(const MESSAGE_ID id,void* data){
   switch (id){
   case ID_SET_ROBOT_PARAMTER:
      Motor_ctrl::get()->update_pid();
      Motor_ctrl::get()->update_model();

      Data_holder::get()->save_parameter();
      break;

   case ID_CLEAR_ODOM:
      Motor_ctrl::get()->clear_odom();
      break;

   case ID_SET_VELOCITY:
      Motor_ctrl::get()->update_velocity();
   default:
      break;
   }
}

void Robot::do_kinmatics_and_calc_odom(){
   Motor_ctrl::get()->do_kinmatics();
   Motor_ctrl::get()->calc_odom();
}

void Robot::update_peripheral_equipment(){
   Peripheral_equipment::get()->get_imu_data();
   Peripheral_equipment::get()->check_joystick();
}