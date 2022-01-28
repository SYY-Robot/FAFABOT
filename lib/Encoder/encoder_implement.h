#ifndef PIBOT_ENCODER_IMP_H_
#define PIBOT_ENCODER_IMP_H_

#include "encoder.h"
#include "arduino_encoder.h"

#define ENCODER_USE_INTERRUPTS
#define ENCODER_OPTIMIZE_INTERRUPTS

//编码器实现  编码器原理具体可以参考https://www.jianshu.com/p/068fb9882655
class EncoderImp : public Encoder
{
public:
    EncoderImp(unsigned char pinA, unsigned char pinB);

    //初始化
    void init();

    //清除编码器
    void clear();

    //获取累计编码器值
    long get_total_count();

    //获取编码器变化值， 用于pid计算
    long get_increment_count_for_dopid();

    //获取编码器变化值， 用于odom计算
    long get_increment_count_for_odom();
private:
    ArduinoEncoder encoder; //使用arduino的编码器库实现， 通过2个中断引脚计数
    long pid_pos, odom_pos;
};

#endif