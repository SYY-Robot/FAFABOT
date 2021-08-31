#pragma once
// 编码器接口
class Encoder
{
public:
    //初始化
    virtual void init()=0;

    //清除编码器
    virtual void clear()=0;

    //获取累计编码器值
    virtual long get_total_count()=0;

    //获取编码器变化值， 用于pid计算
    virtual long get_increment_count_for_dopid()=0;

    //获取编码器变化值， 用于odom计算
    virtual long get_increment_count_for_odom()=0;  
};