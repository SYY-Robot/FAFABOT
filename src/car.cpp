#include "car.h"
#include <Arduino.h>

#include "mu_sensor_megapi.h"
#include "Servo.h"
#include "common_motor_controller.h"

//#include "board.h"

void Car::init() {

   Serial.begin(9600);

   Serial3.begin(9600);

   init_mu_sensor();

   init_motor();

   init_servo();
}

void Car::init_mu_sensor() {
   static MuSensorMegapi mu(MU_ADDRESS);
   musensor = &mu;
   musensor->init(VISION_COLOR_DETECT, MU_COLOR_RED);
}

void Car::init_motor() {
   static CommonMotorController motor1(MOTOR_1_PWM_PIN, MOTOR_1_DIR_A_PIN, MOTOR_1_DIR_B_PIN, MAX_PWM_VALUE);
   static CommonMotorController motor2(MOTOR_2_PWM_PIN, MOTOR_2_DIR_A_PIN, MOTOR_2_DIR_B_PIN, MAX_PWM_VALUE);
   static CommonMotorController motor3(MOTOR_3_PWM_PIN, MOTOR_3_DIR_A_PIN, MOTOR_3_DIR_B_PIN, MAX_PWM_VALUE);
   static CommonMotorController motor4(MOTOR_4_PWM_PIN, MOTOR_4_DIR_A_PIN, MOTOR_4_DIR_B_PIN, MAX_PWM_VALUE);

   motor[0] = &motor1;
   motor[1] = &motor2;
   motor[2] = &motor3;
   motor[3] = &motor4;

   for (int i = 0;i < MOTOR_COUNT;i++) {
      motor[i]->init();
   }
}

void Car::init_servo() {
   static Servo servo1;
   static Servo servo2;

   servo[0] = &servo1;
   servo[1] = &servo2;

   servo[0]->attach(SERVO_1_PIN);
   servo[1]->attach(SERVO_2_PIN);

   servo[0]->write(70);
   servo[1]->write(70);
}

void Car::servo_rotate() {
   if (musensor->Have_someone_in_front_of_camera() == false) {

   }
   servo[0]->write(musensor->GetValue(X));
   servo[1]->write(musensor->GetValue(Y));
}

void Car::motor_rotate() {
   if (Serial3.available()) {
      int flag = Serial3.read();
      switch (flag)
      {
      case 'w':
         motor[0]->control(100);
         motor[1]->control(100);
         motor[2]->control(100);
         motor[3]->control(100);
         break;
      case 'x':
         motor[0]->control(-100);
         motor[1]->control(-100);
         motor[2]->control(-100);
         motor[3]->control(-100);
         break;
      case 'a':
         motor[0]->control(-100);
         motor[1]->control(100);
         motor[2]->control(-100);
         motor[3]->control(100);
         break;
      case 'd':
         motor[0]->control(100);
         motor[1]->control(-100);
         motor[2]->control(100);
         motor[3]->control(-100);
         break;
      case 'q':
         motor[0]->control(0);
         motor[1]->control(100);
         motor[2]->control(0);
         motor[3]->control(100);
         break;
      case 'e':
         motor[0]->control(100);
         motor[1]->control(0);
         motor[2]->control(100);
         motor[3]->control(0);
         break;
      case 'z':
         motor[0]->control(-100);
         motor[1]->control(0);
         motor[2]->control(-100);
         motor[3]->control(-0);
         break;
      case 'c':
         motor[0]->control(0);
         motor[1]->control(-100);
         motor[2]->control(0);
         motor[3]->control(-100);
         break;
      case 'v':
         motor[0]->control(100);
         motor[1]->control(100);
         motor[2]->control(-100);
         motor[3]->control(-100);
         break;
      case 'b':
         motor[0]->control(-100);
         motor[1]->control(-100);
         motor[2]->control(100);
         motor[3]->control(100);
         break;
      
      default:
         motor[0]->control(0);
         motor[1]->control(0);
         motor[2]->control(0);
         motor[3]->control(0);
         break;
      }
   }

}