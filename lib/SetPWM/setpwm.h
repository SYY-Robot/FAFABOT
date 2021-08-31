#pragma once

#include <HardwareTimer.h>
class setpwm{
public:
   setpwm();
   void init(int pin,long hz);
   void pwm(int pwm);

private:
   HardwareTimer *MyTim;
   int pwm_pin;
   int channel;
};