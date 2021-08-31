#include "setpwm.h"
#include <pinmap.h>
#include <pins_arduino.h>
#include <PeripheralPins.h>

setpwm::setpwm(){
   
}

void setpwm::init(int pin,long hz){
   TIM_TypeDef *Instance = (TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(pin), PinMap_PWM);
   channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(pin), PinMap_PWM));

   MyTim = new HardwareTimer(Instance);
   MyTim->setMode(channel,TIMER_OUTPUT_COMPARE_PWM1,pin);
   MyTim->setOverflow(hz,HERTZ_FORMAT);
   MyTim->resume();
}

void setpwm::pwm(int pwm){
   MyTim->setCaptureCompare(channel, pwm, RESOLUTION_10B_COMPARE_FORMAT);
}