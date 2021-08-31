#include "motor_ctrl.h"

#include "board.h"
#include "data_holder.h"

#if MOTOR_CONTROLLER == COMMON_CONTROLLER
#include "common_motor_controller.h"
#endif

#include "encoder_implement.h"
#include "pid.h"

#define __PI  3.1415926535897932384626433832795

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

void Motor_ctrl::init(){
#if MOTOR_COUNT>0
   #if MOTOR_CONTROLLER == COMMON_CONTROLLER
      static CommonMotorController motor1(MOTOR_1_PWM_PIN, MOTOR_1_DIR_A_PIN, MOTOR_1_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
      static EncoderImp encoder1(MOTOR_1_ENCODER_A_PIN, MOTOR_1_ENCODER_B_PIN);
      static PID pid1(&input[0], &feedback[0], float(Data_holder::get()->parameter.kp)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.ki)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.kd)/Data_holder::get()->parameter.ko, MAX_PWM_VALUE);
      motor[0] = &motor1;
      encoder[0] = &encoder1;
      pid[0] = &pid1;
   #endif
#endif

#if MOTOR_COUNT>1
   #if MOTOR_CONTROLLER == COMMON_CONTROLLER
      static CommonMotorController motor2(MOTOR_2_PWM_PIN, MOTOR_2_DIR_A_PIN, MOTOR_2_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
      static EncoderImp encoder2(MOTOR_2_ENCODER_A_PIN, MOTOR_2_ENCODER_B_PIN);
      static PID pid2(&input[1], &feedback[1], float(Data_holder::get()->parameter.kp)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.ki)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.kd)/Data_holder::get()->parameter.ko, MAX_PWM_VALUE);
      motor[1] = &motor2;
      encoder[1] = &encoder2;
      pid[1] = &pid2;
   #endif
#endif

#if MOTOR_COUNT>2
   #if MOTOR_CONTROLLER == COMMON_CONTROLLER
      static CommonMotorController motor3(MOTOR_3_PWM_PIN, MOTOR_3_DIR_A_PIN, MOTOR_3_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
      static EncoderImp encoder3(MOTOR_3_ENCODER_A_PIN, MOTOR_3_ENCODER_B_PIN);
      static PID pid3(&input[2], &feedback[2], float(Data_holder::get()->parameter.kp)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.ki)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.kd)/Data_holder::get()->parameter.ko, MAX_PWM_VALUE);
      motor[2] = &motor3;
      encoder[2] = &encoder3;
      pid[2] = &pid3;
   #endif
#endif

#if MOTOR_COUNT>3
   #if MOTOR_CONTROLLER == COMMON_CONTROLLER
      static CommonMotorController motor4(MOTOR_4_PWM_PIN, MOTOR_4_DIR_A_PIN, MOTOR_4_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
      static EncoderImp encoder4(MOTOR_4_ENCODER_A_PIN, MOTOR_4_ENCODER_B_PIN);
      static PID pid4(&input[3], &feedback[3], float(Data_holder::get()->parameter.kp)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.ki)/Data_holder::get()->parameter.ko, 
                                            float(Data_holder::get()->parameter.kd)/Data_holder::get()->parameter.ko, MAX_PWM_VALUE);
      motor[3] = &motor4;
      encoder[3] = &encoder4;
      pid[3] = &pid4;
   #endif
#endif

#if ROBOT_MODEL == ROBOT_MODEL_2WD_DIFF  
    static Differential diff(Data_holder::get()->parameter.wheel_diameter*0.001/2, Data_holder::get()->parameter.wheel_track*0.001/2);
    model = &diff;
#endif

#if ROBOT_MODEL == ROBOT_MODEL_3WD_OMNI     
    static Omni3 omni3(Data_holder::get()->parameter.wheel_diameter*0.001/2, Data_holder::get()->parameter.wheel_track*0.001/2);
    model = &omni3;
#endif

#if ROBOT_MODEL == ROBOT_MODEL_4WD_DIFF     
    static Differential4WD diff4w(Data_holder::get()->parameter.wheel_diameter*0.001/2, Data_holder::get()->parameter.wheel_track*0.001/2);
    model = &diff4w;
#endif

#if ROBOT_MODEL == ROBOT_MODEL_4WD_OMNI     
    static Omni4 omni4(Data_holder::get()->parameter.wheel_diameter*0.001/2, Data_holder::get()->parameter.wheel_track*0.001/2);
    model = &omni4;
#endif

#if ROBOT_MODEL == ROBOT_MODEL_4WD_MECANUM     
    static Mecanum mecanum(Data_holder::get()->parameter.wheel_diameter*0.001/2, Data_holder::get()->parameter.wheel_track*0.001/2);
    model = &mecanum;
#endif

#if MOTOR_CONTROLLER == COMMON_CONTROLLER
   for (int i=0; i<MOTOR_COUNT; i++) {
        encoder[i]->init();
        motor[i]->init();
    }
#endif

   do_kinmatics_flag = false;

   memset(&odom, 0, sizeof(odom));
   memset(&input, 0, sizeof(input));
   memset(&feedback, 0, sizeof(feedback));

   last_velocity_command_time = 0;
}

void Motor_ctrl::clear_odom(){

#if MOTOR_CONTROLLER == COMMON_CONTROLLER
   for(int i=0;i<MOTOR_COUNT;i++){
      encoder[i]->clear();
   }
#endif

   memset(&odom,0,sizeof(odom));
   memset(&Data_holder::get()->odom,0,sizeof(Data_holder::get()->odom));
}

void Motor_ctrl::update_velocity(){
   short vx = min(max(Data_holder::get()->velocity.v_liner_x, -(short(Data_holder::get()->parameter.max_v_liner_x))), short(Data_holder::get()->parameter.max_v_liner_x));
   short vy = min(max(Data_holder::get()->velocity.v_liner_y, -(short(Data_holder::get()->parameter.max_v_liner_y))), short(Data_holder::get()->parameter.max_v_liner_y));
   short vz = min(max(Data_holder::get()->velocity.v_angular_z, -(short(Data_holder::get()->parameter.max_v_angular_z))), short(Data_holder::get()->parameter.max_v_angular_z));

   float vel[3]={vx/100.0, vy/100.0 ,vz/100.0};
   float motor_speed[MOTOR_COUNT];
   model->motion_solver(vel,motor_speed);

#if MOTOR_CONTROLLER == COMMON_CONTROLLER
   for(int i=0;i<MOTOR_COUNT;i++){
      input[i] = motor_speed[i]*Data_holder::get()->parameter.encoder_resolution*Data_holder::get()->parameter.motor_ratio/(2*__PI)*Data_holder::get()->parameter.do_pid_interval*0.001;
   }
#endif

   last_velocity_command_time = Board::get()->get_tick_count();

   do_kinmatics_flag = true;
}

void Motor_ctrl::do_kinmatics(){
   if(!do_kinmatics_flag){
      for(int i=0;i<MOTOR_COUNT;i++){
      #if MOTOR_CONTROLLER == COMMON_CONTROLLER
         encoder[i]->get_increment_count_for_dopid();
      #endif
         pid[i]->clear();
      }
      return;
   }

   static unsigned long last_millis=0;
   if(Board::get()->get_tick_count()-last_millis>=Data_holder::get()->parameter.do_pid_interval){
      last_millis = Board::get()->get_tick_count();

      for(int i=0;i<MOTOR_COUNT;i++){
      #if MOTOR_CONTROLLER == COMMON_CONTROLLER
         feedback[i] = encoder[i]->get_increment_count_for_dopid();
      #endif
      }

      bool stoped=true;

      for(int i=0;i<MOTOR_COUNT;i++){
         if(input[i] != 0 || feedback[i] != 0){
            stoped = false;
            break;
         }
      }

      short output[MOTOR_COUNT]={0};

      if(stoped){
         for(int i=0;i<MOTOR_COUNT;i++){
            output[i] = 0;
         }
         do_kinmatics_flag = false;
      }
      else{
         for(int i=0;i<MOTOR_COUNT;i++){
         #if MOTOR_CONTROLLER == COMMON_CONTROLLER
            output[i] = pid[i]->compute(Data_holder::get()->parameter.do_pid_interval*0.001);
         #endif
         }
      }
      
      for(int i=0;i<MOTOR_COUNT;i++){
         Data_holder::get()->pid_data.input[i] = int(input[i]);
         Data_holder::get()->pid_data.output[i] = int(feedback[i]);
      }

      long elapsed_ms = Board::get()->get_tick_count()-last_velocity_command_time;

      if(elapsed_ms > Data_holder::get()->parameter.cmd_last_time){
         memset(input,0,sizeof(input));
         if(elapsed_ms > Data_holder::get()->parameter.cmd_last_time*2){
            memset(output,0,sizeof(output));
         }
      }

   #if MOTOR_CONTROLLER == COMMON_CONTROLLER
      for(int i=0;i<MOTOR_COUNT;i++){
         motor[i]->control(output[i]);
      }
   #endif


   }
}


#define CALC_ODOM_INTERVAL 100
void Motor_ctrl::calc_odom(){
   static unsigned long last_millis=0;
   
   if(Board::get()->get_tick_count()-last_millis>=CALC_ODOM_INTERVAL){
      last_millis = Board::get()->get_tick_count();

      float dis[MOTOR_COUNT] = {0};
      for(int i=0;i<MOTOR_COUNT;i++){
      #if MOTOR_CONTROLLER == COMMON_CONTROLLER
         dis[i] = encoder[i]->get_increment_count_for_odom()*__PI*Data_holder::get()->parameter.wheel_diameter*0.001/Data_holder::get()->parameter.encoder_resolution/Data_holder::get()->parameter.motor_ratio;
      #endif
      }

      model->get_odom(&odom,dis,CALC_ODOM_INTERVAL);

      Data_holder::get()->odom.v_liner_x = odom.vel_x*100;
      Data_holder::get()->odom.v_liner_y = odom.vel_y*100;
      Data_holder::get()->odom.v_angular_z = odom.vel_z*100;
      Data_holder::get()->odom.x = odom.x*100;
      Data_holder::get()->odom.y = odom.y*100;
      Data_holder::get()->odom.yaw = long(odom.z*100)%628;
   }

   
}

void Motor_ctrl::update_pid(){
   for (int i=0; i<MOTOR_COUNT; i++) {
            pid[i]->update(float(Data_holder::get()->parameter.kp)/Data_holder::get()->parameter.ko, 
                           float(Data_holder::get()->parameter.ki)/Data_holder::get()->parameter.ko, 
                           float(Data_holder::get()->parameter.kd)/Data_holder::get()->parameter.ko, 
                           MAX_PWM_VALUE);
        }
}

void Motor_ctrl::update_model(){
   model->set(Data_holder::get()->parameter.wheel_diameter*0.001/2, Data_holder::get()->parameter.wheel_track*0.001/2);
}