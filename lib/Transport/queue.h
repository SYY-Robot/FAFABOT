#pragma once

// 一个ringbuffer的接口
class Queue
{
public:
    // 生产数据
    virtual bool put(unsigned char ch)=0;

    // 消费数据
    virtual bool get(unsigned char& ch)=0;

    // 可用数据
    virtual unsigned short size()=0;

    // 占用大小
    virtual unsigned short max_size()=0;
};