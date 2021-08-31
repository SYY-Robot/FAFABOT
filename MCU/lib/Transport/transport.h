#pragma once

// 传输数据接口， 继承改接口实现一个用于传输的接口
class Transport
{
public:
    // 初始化
    virtual bool init()=0;

    // 读取数据
    virtual bool read(unsigned char& ch)=0;

    // 写入数据
    virtual void write(unsigned char* data, unsigned char len) = 0;
};