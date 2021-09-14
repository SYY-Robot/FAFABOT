#include <Arduino.h>
#include "config.h"
#include "common_motor_controller.h"
int flag;
#define pwm_value 150
CommonMotorController motor1(MOTOR_1_PWM_PIN, MOTOR_1_DIR_A_PIN, MOTOR_1_DIR_B_PIN, MAX_PWM_VALUE);
CommonMotorController motor2(MOTOR_2_PWM_PIN, MOTOR_2_DIR_A_PIN, MOTOR_2_DIR_B_PIN, MAX_PWM_VALUE);
CommonMotorController motor3(MOTOR_3_PWM_PIN, MOTOR_3_DIR_A_PIN, MOTOR_3_DIR_B_PIN, MAX_PWM_VALUE);
CommonMotorController motor4(MOTOR_4_PWM_PIN, MOTOR_4_DIR_A_PIN, MOTOR_4_DIR_B_PIN, MAX_PWM_VALUE);

void fwd();
void back();
void left();
void right();
void top_right();
void top_left();
void bottom_right();
void bottom_left();
void middle_right();
void middle_left();
void brake();


void setup() {
  Serial.begin(9600);
  motor1.init();
  motor2.init();
  motor3.init();
  motor4.init();
}

void loop() {
  if(Serial.available()){
      flag = Serial.read();
  }
  switch(flag){
      case 'w': 
              fwd();
              break;
      case 'x': 
              back();
              break;
      case 'a': 
              middle_left();
              break;
      case 'd': 
              middle_right();
              break;
      case 'q': 
              top_left();
              break;
      case 'e': 
              top_right();
              break;
      case 'z': 
              bottom_left();
              break;
      case 'c': 
              bottom_right();
              break;
      case 'v': 
              right();
              break;
      case 'b': 
              left();
              break;
      default:
              brake();
  }
}

void fwd(){
  motor1.control(pwm_value);
  motor2.control(pwm_value);
  motor3.control(pwm_value);
  motor4.control(pwm_value);
}

void back(){
  motor1.control(-pwm_value);
  motor2.control(-pwm_value);
  motor3.control(-pwm_value);
  motor4.control(-pwm_value);
}

void left(){
  motor1.control(-pwm_value);
  motor2.control(-pwm_value);
  motor3.control(pwm_value);
  motor4.control(pwm_value);
}

void right(){
  motor1.control(pwm_value);
  motor2.control(pwm_value);
  motor3.control(-pwm_value);
  motor4.control(-pwm_value);
}

void top_right(){
  motor1.control(0);
  motor2.control(pwm_value);
  motor3.control(pwm_value);
  motor4.control(0);
}

void top_left(){
  motor1.control(pwm_value);
  motor2.control(0);
  motor3.control(0);
  motor4.control(pwm_value);
}

void bottom_right(){
  motor1.control(-pwm_value);
  motor2.control(0);
  motor3.control(0);
  motor4.control(-pwm_value);
}

void bottom_left(){
  motor1.control(0);
  motor2.control(-pwm_value);
  motor3.control(-pwm_value);
  motor4.control(0);
}

void middle_right(){
  motor1.control(-pwm_value);
  motor2.control(pwm_value);
  motor3.control(pwm_value);
  motor4.control(-pwm_value);
}

void middle_left(){
  motor1.control(pwm_value);
  motor2.control(-pwm_value);
  motor3.control(-pwm_value);
  motor4.control(pwm_value);
}

void brake(){
  motor1.control(0);
  motor2.control(0);
  motor3.control(0);
  motor4.control(0);
}