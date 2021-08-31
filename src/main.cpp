#include <Arduino.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

// define HUSKYLENS
HUSKYLENS huskylens;
SoftwareSerial mySerial(9, 11); //RX TX
void printResult(HUSKYLENSResult result);

int motor_speed;

// Motor控制關
void forward()
{
  digitalWrite(MOTOR_1_AB_PIN, LOW);
  analogWrite(MOTOR_1_PWM_PIN, motor_speed);
  digitalWrite(MOTOR_2_AB_PIN, HIGH);
  analogWrite(MOTOR_2_PWM_PIN, motor_speed);
}

void stop2()
{
  analogWrite(MOTOR_1_PWM_PIN, 0);
  analogWrite(MOTOR_2_PWM_PIN, 0);
}

void left()
{
  digitalWrite(MOTOR_1_AB_PIN, LOW);
  analogWrite(MOTOR_1_PWM_PIN, 0);
  digitalWrite(MOTOR_2_AB_PIN, HIGH);
  analogWrite(MOTOR_2_PWM_PIN, motor_speed);
}

void backward()
{
  digitalWrite(MOTOR_1_AB_PIN, HIGH);
  analogWrite(MOTOR_1_PWM_PIN, motor_speed);
  digitalWrite(MOTOR_2_AB_PIN, LOW);
  analogWrite(MOTOR_2_PWM_PIN, motor_speed);
}

void right()
{
  digitalWrite(MOTOR_1_AB_PIN, LOW);
  analogWrite(MOTOR_1_PWM_PIN, motor_speed);
  digitalWrite(MOTOR_2_AB_PIN, LOW);
  analogWrite(MOTOR_2_PWM_PIN, 0);
}

// debug HUSKYLENS
void printResult(HUSKYLENSResult result)
{
  if (result.command == COMMAND_RETURN_BLOCK)
  {
    Serial.println(String() + F("Block:xCenter=") + result.xCenter + F(",yCenter=") + result.yCenter + F(",width=") + result.width + F(",height=") + result.height + F(",ID=") + result.ID);
  }
  else if (result.command == COMMAND_RETURN_ARROW)
  {
    Serial.println(String() + F("Arrow:xOrigin=") + result.xOrigin + F(",yOrigin=") + result.yOrigin + F(",xTarget=") + result.xTarget + F(",yTarget=") + result.yTarget + F(",ID=") + result.ID);
  }
  else
  {
    Serial.println("Object unknown!");
  }
}

/*
0,0------------------------------X
|                                |
|               80               |
|      120    160,120   200      |
|               130              |
|                                |
Y---------------------------320,240
*/

void do_robot(HUSKYLENSResult result)
{
  if (result.xCenter < 120)
  {
    left();
  }

  else if (result.xCenter > 200)
  {
    right();
  }

  else if ((result.xCenter >= 120) && (result.xCenter <= 200))
  {
    if (result.width <= 80)
    {
      forward();
    }
    else if (result.width >= 130)
    {
      backward();
    }
    else
    {
      stop2();
    }
  }
  else
  {
    stop2();
  }
}

void setup()
{
  // 通訊初始化
  Serial.begin(9600);   // debug
  mySerial.begin(9600); // 跟二哈

  // 馬達初始化
  pinMode(MOTOR_1_AB_PIN, OUTPUT);
  pinMode(MOTOR_1_PWM_PIN, OUTPUT);
  pinMode(MOTOR_2_AB_PIN, OUTPUT);
  pinMode(MOTOR_2_PWM_PIN, OUTPUT);

  // 二哈初始化
  while (!huskylens.begin(mySerial))
  {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \" Protocol Type \" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
}

void loop()
{
  if (!huskylens.request())
  {
    Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    stop2();
  }
  else if (!huskylens.isLearned())
  {
    Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    stop2();
  }
  else if (!huskylens.available())
  {
    Serial.println(F("No block or arrow appears on the screen!"));
    stop2();
  }
  else
  {
    Serial.println(F("###########"));
    while (huskylens.available())
    {
      HUSKYLENSResult result = huskylens.read();
      printResult(result);
      motor_speed = 50;
      do_robot(result);
    }
  }
}