#include "Arduino.h"
#include "common_motor_controller.h"

CommonMotorController::CommonMotorController(int _pwm_pin, int _dir_pinA, int _dir_pinB, unsigned short _pwm_max, unsigned long _pwm_fre): 
                pwm_pin(_pwm_pin), dir_pinA(_dir_pinA), dir_pinB(_dir_pinB), pwm_max(_pwm_max), pwm_fre(_pwm_fre)
{
}

void CommonMotorController::init()
{
    pinMode(dir_pinA, OUTPUT);
    pinMode(dir_pinB, OUTPUT);
    //Board::get()->pwm_init(PWM_FREQUENCE);
    Setpwm.init(pwm_pin,pwm_fre);
    last_pwm = 0;

    digitalWrite(dir_pinA, 1);
    digitalWrite(dir_pinB, 1);
    //Board::get()->pwm_output(pwm_pin, 0);
    Setpwm.pwm(0);
}

void CommonMotorController::control(short pwm)
{
    if (last_pwm == pwm)
        return;
    last_pwm = pwm;

    if (pwm > 0) {
        digitalWrite(dir_pinA, 1);
        digitalWrite(dir_pinB, 0);

        if (pwm <= short(pwm_max))
            //Board::get()->pwm_output(pwm_pin, pwm);
            Setpwm.pwm(pwm);
        else
            //Board::get()->pwm_output(pwm_pin, pwm_max);
            Setpwm.pwm(pwm_max);
    } else if (pwm<0) {
        digitalWrite(dir_pinA, 0);
        digitalWrite(dir_pinB, 1);

        if (pwm >= -short(pwm_max))
            //Board::get()->pwm_output(pwm_pin, -pwm);
            Setpwm.pwm(-pwm);
        else
            //Board::get()->pwm_output(pwm_pin, pwm_max);
            Setpwm.pwm(pwm_max);
    } else {
        digitalWrite(dir_pinA, 1);
        digitalWrite(dir_pinB, 1);
        Setpwm.pwm(0);
        //Board::get()->pwm_output(pwm_pin, 0);
    }
}