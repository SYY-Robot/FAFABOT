#include "simple_dataframe_slave.h"
#include "data_holder.h"
#include <stdio.h>
#include "transport.h"

Simple_dataframe::Simple_dataframe(Transport* _trans) : trans(_trans)
{
    recv_state = STATE_RECV_FIX;
}

bool Simple_dataframe::init()
{
    return true;
}

bool Simple_dataframe::data_recv(unsigned char c)
{
    //printf("%02x", c);
    switch (recv_state)
    {// 根据当前接收的状态解析数据存储到active_rx_msg
    case STATE_RECV_FIX:
        if (c == FIX_HEAD) {
            memset(&active_rx_msg,0, sizeof(active_rx_msg));
            active_rx_msg.head.flag = c;
            active_rx_msg.check += c;

            recv_state = STATE_RECV_ID;//更新接收id状态
        }
        break;
    case STATE_RECV_ID:
        if (c < ID_MESSGAE_MAX) {
            active_rx_msg.head.msg_id = c;
            active_rx_msg.check += c;
            recv_state = STATE_RECV_LEN;//更新接收len状态
        }
        else
            recv_state = STATE_RECV_FIX;
        break;
    case STATE_RECV_LEN:
        active_rx_msg.head.length =c;
        active_rx_msg.check += c;
        if (active_rx_msg.head.length==0)
            recv_state = STATE_RECV_CHECK;//没有data段，直接更新到接收checksum状态
        else
            recv_state = STATE_RECV_DATA;//更新接收数据
        break;
    case STATE_RECV_DATA:
        active_rx_msg.data[active_rx_msg.recv_count++] = c;
        active_rx_msg.check += c;
        if (active_rx_msg.recv_count >=active_rx_msg.head.length)
            recv_state  = STATE_RECV_CHECK;//接收完成，更新到接收checksum状态
        break;
    case STATE_RECV_CHECK:
        recv_state = STATE_RECV_FIX;
        if (active_rx_msg.check == c) { //校验码校验
            return true;  //返回true 标识接收到一帧正确的数据
        }
        break;
    default:
        recv_state = STATE_RECV_FIX;
    }

    return false;
}

//接收到一帧正确的数据后被调用
bool Simple_dataframe::data_parse()
{
    MESSAGE_ID id = (MESSAGE_ID)active_rx_msg.head.msg_id;

    //pb_printf("\r\ndata_parse:id=%d", id);

    Data_holder* dh = Data_holder::get();
    //更新接收到的id执行响应操作，一般分为读取，写入，和执行动作3个操作
    switch (id) {
    case ID_GET_VERSION://读取版本
        send_message(id, (unsigned char*)&dh->firmware_info, sizeof(dh->firmware_info));
        break;
    case ID_SET_ROBOT_PARAMTER://设置参数
        memcpy(&dh->parameter, active_rx_msg.data, sizeof(dh->parameter));
        send_message(id);
        break;
    case ID_GET_ROBOT_PARAMTER://读取参数
        send_message(id, (unsigned char*)&dh->parameter, sizeof(dh->parameter));
        break;
    case ID_CLEAR_ODOM://清除里程信息，
        send_message(id); //回复数据，如果需要对改id的关注需要，调用register_notify接口注册对改id的关注
        break;
    case ID_SET_VELOCITY: //设置数据
        memcpy(&dh->velocity, active_rx_msg.data, sizeof(dh->velocity));
        send_message(id);
        break;
    case ID_GET_ODOM:  //获取里程计
        send_message(id, (unsigned char*)&dh->odom, sizeof(dh->odom));
        break;
    case ID_GET_PID_DATA://获取用于调试的pid信息
        send_message(id, (unsigned char*)&dh->pid_data, sizeof(dh->pid_data));
        break;
    case ID_GET_IMU_DATA://获取imu数据
        send_message(id, (unsigned char*)&dh->imu_data, sizeof(dh->imu_data));
        break;
     case ID_SET_LCD_STATUS://修改LCD內容
        memcpy(&dh->lcd_status.robot_ip,active_rx_msg.data,sizeof(dh->lcd_status.robot_ip));
        send_message(id);
        break;
    case ID_GET_SONA_DATA://獲取機器人整體狀態
        send_message(id,(unsigned char*)&dh->sona_data,sizeof(dh->sona_data));
        break;
    case ID_GET_ROBOT_STATUS://機器人狀態
        send_message(id,(unsigned char*)&dh->robot_status,sizeof(dh->robot_status));
        break;
    default:
        break;
    }

    // 根据已经注册的notify，回调update通知观察者
    if (id < ID_MESSGAE_MAX && nf[id] != 0)                      //imu的值 9axis nf[id] != 0)
        nf[id]->update(id, &dh);

    return true;
}

bool Simple_dataframe::send_message(const MESSAGE_ID id)
{
    Message msg(id);

    send_message(&msg);

    return true;
}

bool Simple_dataframe::send_message(const MESSAGE_ID id, unsigned char* data, unsigned char len)
{
    Message msg(id, data, len);

    send_message(&msg);

    return true;
}

bool Simple_dataframe::send_message(Message* msg)
{
    if (trans == 0)
        return true;

    // 回复信息，通过使用的trans接口写数据    
    trans->write((unsigned char*)msg, sizeof(msg->head)+msg->head.length+1);

    return true;
}