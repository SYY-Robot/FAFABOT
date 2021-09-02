#include <Arduino.h>
#include "config.h"
#include "common_motor_controller.h"
#include <Pixetto.h>
Pixetto ss(PA3, PA2);
CommonMotorController motor1(MOTOR_1_PWM_PIN, MOTOR_1_DIR_A_PIN, MOTOR_1_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
CommonMotorController motor2(MOTOR_2_PWM_PIN, MOTOR_2_DIR_A_PIN, MOTOR_2_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
CommonMotorController motor3(MOTOR_3_PWM_PIN, MOTOR_3_DIR_A_PIN, MOTOR_3_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
CommonMotorController motor4(MOTOR_4_PWM_PIN, MOTOR_4_DIR_A_PIN, MOTOR_4_DIR_B_PIN, MAX_PWM_VALUE,PWM_FREQUENCE);
int time=0;

void forward();
void stop();
void left();
void backward();
void right();
void setup()
{
  ss.begin();

  ss.enableFunc(Pixetto::FUNC_COLOR_DETECTION);
  Serial1.begin(9600);
  motor1.init();
  motor2.init();
  motor3.init();
  motor4.init();
  
}


void loop()
{
  if (ss.isDetected()) {
    time = millis();
    /*Serial1.print("x:");
    Serial1.println(ss.getPosX());
    Serial1.print("y:");
    Serial1.println(ss.getPosY());
    Serial1.print("w:");
    Serial1.println(ss.getWidth());*/
    if (ss.getTypeID() == Pixetto::COLOR_PURPLE){
      if(ss.getPosX() >= 60){
        //Serial1.println("right");
        right();
      }
      else if(ss.getPosX() <= 15){
        //Serial1.println("left");
        left();
      }
      else if(ss.getPosX() >= 15 && ss.getPosX() <= 60 ){
        if(ss.getWidth() < 18){
          //Serial1.println("forward");
          forward();
        }
        else if(ss.getWidth() > 40){
          //Serial1.println("backward");
          backward();
        }
        else{
          //Serial1.println("stop");
          stop();
        }
      }
    }
    else{
      //stop();
    }
  }
  if(millis() - time > 300){
    //Serial1.println("stop");
    stop();
  }
}

void forward(){
  motor1.control(200);
  motor2.control(200);
  motor3.control(200);
  motor4.control(200);
}

void backward(){
  motor1.control(-200);
  motor2.control(-200);
  motor3.control(-200);
  motor4.control(-200);
}

void right(){
  motor1.control(200);
  motor2.control(-200);
  motor3.control(200);
  motor4.control(-200);
}

void left(){
  motor1.control(-200);
  motor2.control(200);
  motor3.control(-200);
  motor4.control(200);
}

void stop(){
  motor1.control(0);
  motor2.control(0);
  motor3.control(0);
  motor4.control(0);
}