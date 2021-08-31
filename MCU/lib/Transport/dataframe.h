#pragma once

// 协议接口类

//消息id定义
enum MESSAGE_ID
{
    ID_GET_VERSION = 0,             // 获取version信息
    ID_SET_ROBOT_PARAMTER = 1,      // 设置机器人参数
    ID_GET_ROBOT_PARAMTER = 2,      // 获取机器人参数
    ID_CLEAR_ODOM = 3,              // 清除里程计
    ID_SET_VELOCITY = 4,            // 下发速度值
    ID_GET_ODOM = 5,                // 获取里程计值
    ID_GET_PID_DATA = 6,            // 获取PID值的输入和反馈（用于PID参数调节)
    ID_GET_IMU_DATA = 7,            // 获取imu值
    ID_SET_LCD_STATUS = 8,          // 修改LCD顯示內容
    ID_GET_SONA_DATA = 9,           //機器人狀態(防撞條狀態、電池電壓、超音波數值)
    ID_GET_ROBOT_STATUS = 10,       //更新機器人狀態
    ID_MESSGAE_MAX
};

// 通知接口
class Notify
{
public:
    virtual void update(const MESSAGE_ID id, void* data) = 0;
};

// 协议接口， 继承改接口实现一种协议
class Dataframe
{
public:
    //初始化
    virtual bool init()=0;

    // 注册notify， 对需要关注的id添加关注
    virtual void register_notify(const MESSAGE_ID id, Notify* _nf)=0;

    // 数据接收接口
    virtual bool data_recv(unsigned char c)=0;

    // 解析协议
    virtual bool data_parse()=0;

    // 用于主动交互信息 预留
    virtual bool interact(const MESSAGE_ID id)=0;
};
