#include <Arduino.h>
#include "robot.h"
void setup() { 

   Robot::get()->init();
}

void loop() {

   Robot::get()->check_command();

   Robot::get()->do_kinmatics_and_calc_odom();

   Robot::get()->update_peripheral_equipment();
}