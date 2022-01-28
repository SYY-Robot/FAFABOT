#ifndef PIBOT_SIMPLE_DATAFRAME_SLAVE_H_
#define PIBOT_SIMPLE_DATAFRAME_SLAVE_H_

#include "simple_dataframe.h"

class Transport;

// 一个简单的协议实现  详见https://www.jianshu.com/p/1dc5f9e2f90f
class Simple_dataframe : public Dataframe
{
public:
    /* 构造函数
    * @param trans 用于传输的接口
    */
    Simple_dataframe(Transport* trans=0);
    
    // 注册通知接口实现， 保存notify到数组以作映射
    void register_notify(const MESSAGE_ID id, Notify* _nf){
        if (id >= ID_MESSGAE_MAX)
            return;
        nf[id] = _nf;
    }

    // 初始化
    bool init();

    // 数据接口实现
    bool data_recv(unsigned char c);

    // 数据解析实现
    bool data_parse();

    //预留
    bool interact(const MESSAGE_ID id) {return true;};
private:
    bool send_message(const MESSAGE_ID id);
    bool send_message(const MESSAGE_ID id, unsigned char* data, unsigned char len);
    bool send_message(Message* msg);
private:
    Notify* nf[ID_MESSGAE_MAX]; // 用于保存notify的指针的数组 

    Message active_rx_msg;      //保存当前接受的msg信息

    RECEIVE_STATE recv_state;   //当前接收数据所处状态
    Transport* trans;           // 传输的接口
};
#endif