#pragma once

class CommonMotorController
{
  public:
    CommonMotorController(int pwm_pin, int dir_pinA, int dir_pinB, unsigned short _pwm_max);
    ~CommonMotorController() {}
    void init();
    void control(short pwm);
  private:
    int pwm_pin, dir_pinA, dir_pinB;
    unsigned short pwm_max;
    unsigned long pwm_fre;
    short last_pwm;
};
