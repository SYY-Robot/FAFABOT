#include "Arduino.h"
#include "common_motor_controller.h"

CommonMotorController::CommonMotorController(int _pwm_pin, int _dir_pinA, int _dir_pinB, unsigned short _pwm_max): 
                pwm_pin(_pwm_pin), dir_pinA(_dir_pinA), dir_pinB(_dir_pinB), pwm_max(_pwm_max)
{
}

void CommonMotorController::init()
{
    pinMode(dir_pinA, OUTPUT);
    pinMode(dir_pinB, OUTPUT);
    pinMode(pwm_pin,OUTPUT);
    last_pwm = 0;

    digitalWrite(dir_pinA, 1);
    digitalWrite(dir_pinB, 1);
    analogWrite(pwm_pin,0);
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
            analogWrite(pwm_pin,pwm);
        else
            analogWrite(pwm_pin,pwm_max);
    } else if (pwm<0) {
        digitalWrite(dir_pinA, 0);
        digitalWrite(dir_pinB, 1);

        if (pwm >= -short(pwm_max))
            analogWrite(pwm_pin,-pwm);
        else
            analogWrite(pwm_pin,pwm_max);
    } else {
        digitalWrite(dir_pinA, 1);
        digitalWrite(dir_pinB, 1);
        analogWrite(pwm_pin,pwm_max);
    }
}