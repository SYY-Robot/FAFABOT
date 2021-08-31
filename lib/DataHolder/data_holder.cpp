#include "data_holder.h"
#include "board.h"

//从flash中读取参数
void Data_holder::load_parameter()
{
    Board::get()->get_config((unsigned char*)&parameter, sizeof(parameter));

    check_parameter();
}

//保存参数到flash
void Data_holder::save_parameter()
{
    Board::get()->set_config((unsigned char*)&parameter, sizeof(parameter));

    check_parameter();
}
    
void Data_holder::check_parameter()
{
    if (parameter.ko == 0) {
        parameter.ko = 1;
    }
    
    if (parameter.do_pid_interval == 0) {
        parameter.do_pid_interval = 10;
    }

    if (parameter.motor_ratio == (unsigned short)-1 || parameter.motor_ratio == 0) {
        parameter.motor_ratio = 1;
    }

    parameter.model_type = MODEL_TYPE_2WD_DIFF; // in arduino fix 2wd
}

    

