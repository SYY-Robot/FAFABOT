#include <Arduino.h>
#include "peripheral_equipment.h"
#include "data_holder.h"
#include "board.h"
#include "motor_ctrl.h"

#if IMU_ENABLE
#include "GY65.h"
#endif

#if JOYSTICK_ENABLE
#include "joystick.h"
#endif

void Peripheral_equipment::init(){

   init_imu();

   init_joystick();

}

void Peripheral_equipment::init_imu(){
#if IMU_ENABLE
    // 根據配置的IMU類型使用對於的IMU實例來初始化
    if (Data_holder::get()->parameter.imu_type == IMU_TYPE_GY65) {
        static GY65 gy65;
        imu = &gy65;
    } else {
        imu = NULL;
    }

    if (imu != NULL) {
        imu->init();
    }
#endif
}

void Peripheral_equipment::init_joystick(){
#if JOYSTICK_ENABLE
    static Joystick joy;
    joystick = &joy;
    joystick->init();
#endif
}

#define CALC_IMU_INTERVAL 100
void Peripheral_equipment::get_imu_data(){
#if IMU_ENABLE
    if(imu == NULL){
        return;
    }

    static unsigned long last_millis=0;
    if(Board::get()->get_tick_count()-last_millis>=CALC_IMU_INTERVAL){
        last_millis = Board::get()->get_tick_count();
        imu->get_data(Data_holder::get()->imu_data);
    }
#endif
}

#define CHECK_JOYSTICK_INTERVAL 100
//檢測joystick
void Peripheral_equipment::check_joystick()
{
#if JOYSTICK_ENABLE
    static unsigned long last_millis=0;
    short liner_x=0, liner_y=0, angular_z=0;
    if (Board::get()->get_tick_count()-last_millis>=CHECK_JOYSTICK_INTERVAL) {
        last_millis = Board::get()->get_tick_count();
        // 按照设置的间隔间隔 更新期望速度
        if (joystick->check(liner_x, liner_y, angular_z)) {
            Data_holder::get()->velocity.v_liner_x = liner_x;
            Data_holder::get()->velocity.v_liner_y = liner_y;
            Data_holder::get()->velocity.v_angular_z = angular_z;
            Motor_ctrl::get()->update_velocity();
        }
    }
#endif
}