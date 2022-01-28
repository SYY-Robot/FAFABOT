#include <Arduino.h>
#include "car.h"

void setup() {
   Car::get()->init();
   pinMode(3,OUTPUT);
   digitalWrite(3,HIGH);
}

void loop() {
   Car::get()->servo_rotate();

   Car::get()->motor_rotate();
}