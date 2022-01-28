#ifndef DATA_HOLDER_H_
#define DATA_HOLDER_H_

#include <string.h>

#pragma pack(1)

struct Robot_firmware{
    char version[16]; //版本
    char time[16];  //建立時間
};

enum IMU_TYPE
{
   IMU_TYPE_MPU6050 = 65,
   IMU_TYPE_GY85 = 69,
   IMU_TYPE_GY87 = 71,
   IMU_TYPE_MPU9250 = 92,
   IMU_TYPE_BMI160 = 16,
};

enum MODEL_TYPE
{
   MODEL_TYPE_2WD_DIFF = 1,
   MODEL_TYPE_4WD_DIFF = 2,
   MODEL_TYPE_3WD_OMNI = 11,
   MODEL_TYPE_4WD_OMNI = 12,
   MODEL_TYPE_4WD_MECANUM = 21,
};

struct Robot_parameter
{
    union
    {
        char buff[64];
        struct
        {
            unsigned short wheel_diameter;      //轮子直径  mm
            unsigned short wheel_track;         //差分：轮距， 三全向轮：直径，四全向：前后轮距+左右轮距 mm
            unsigned short encoder_resolution;  //编码器分辨率
            unsigned char do_pid_interval;      //pid间隔 (ms)
            unsigned short kp;
            unsigned short ki;
            unsigned short kd;
            unsigned short ko;                  //pid参数比例
            unsigned short cmd_last_time;       //命令持久时间ms 超过该时间会自动停止运动
            unsigned short max_v_liner_x;       // 最大x线速度
            unsigned short max_v_liner_y;       // 最大y线速度
            unsigned short max_v_angular_z;     // 最大角速度
            unsigned char imu_type;             // imu类型
            unsigned short motor_ratio;         // 电机减速比
		    unsigned char model_type;           // 运动模型类型 参见MODEL_TYPE
            unsigned short sona_distance;       // 超音波判斷距離，兩顆相同，0為關閉
        };
    };
};

struct Robot_velocity
{
    short v_liner_x;        //线速度 前>0 cm/s
    short v_liner_y;        //差分轮 为0  cm/s
    short v_angular_z;      //角速度 左>0 0.01rad/s  100 means 1 rad/s
};

struct Robot_odom
{
    short v_liner_x;        //线速度 前>0 后<0  cm/s
    short v_liner_y;        //差分轮 为0        cm/s
    short v_angular_z;      //角速度 左>0 右<0  0.01rad/s   100 means 1 rad/s
    long x;                 //里程计坐标x       cm (这里long为4字节，下同)
    long y;                 //里程计坐标y       cm
    short yaw;              //里程计航角        0.01rad     100 means 1 rad
};

struct Robot_pid_data{
    long input[4];          //各轮子驱动输入值
    long output[4];         //个轮子输出值
};

struct Robot_led_status{
    short ledNum;            //LED編號，NO.1~NO.4條LED
    short led_brightness;    //LED亮度
    short led_speed;         //LED閃爍速度
    short led_color;         //LED顏色
    short led_mode;          //LED亮的模式
};

struct Robot_sona
{
    union 
    {
        unsigned char sona_data[8];
        struct
        {
            unsigned char sona1_data;
            unsigned char sona2_data;
            unsigned char sona3_data;
            unsigned char sona4_data;
            unsigned char sona5_data;
            unsigned char sona6_data;
            unsigned char sona7_data;
            unsigned char sona8_data;
        } sona;  
    };
};

struct Robot_status{
    bool  bumper_status;
    float mcu_voltage;
};


struct Robot_lcd_status{
    char name[16];          //機器人名字
    char robot_ip[16];      //機器人ip位置
};


#pragma pack(0)

//全局数据保存
class Data_holder
{
public:

    static Data_holder* get() {
        static Data_holder dh;
        return &dh;
    }

    void load_parameter();

    void save_parameter();

private:
    Data_holder() {
        strncpy((char*)&firmware_info.version, "V0.Rabbit.1", strlen("V0.Rabbit.1"));
        strncpy((char*)&firmware_info.time, "2021/06/08", strlen("2021/06/08"));
        memset(&parameter, 0, sizeof(struct Robot_parameter));
        memset(&velocity, 0, sizeof(struct Robot_velocity));
        memset(&odom, 0, sizeof(struct Robot_odom));
        memset(&pid_data, 0, sizeof(struct Robot_pid_data));
        memset(imu_data,0,sizeof(imu_data));
        memset(sona_data,0,sizeof(sona_data));
        memset(&robot_status,0,sizeof(struct Robot_status));
        memset(&led_status, 0, sizeof(struct Robot_led_status));
        strncpy((char*)&lcd_status.name,"Waiting...",strlen("Waiting..."));
        strncpy((char*)&lcd_status.robot_ip,"11111",strlen("111111"));
    }

    void check_parameter();
public:
    struct Robot_firmware       firmware_info;   //版本信息
    struct Robot_parameter      parameter;       //参数
    struct Robot_velocity       velocity;        // 速度
    struct Robot_odom           odom;            //里程
    struct Robot_pid_data       pid_data;        //pid输入输出
    float imu_data[9];                      //imu的值 9axis
    unsigned char sona_data[8];
    struct Robot_led_status     led_status;      //led狀態控制
    struct Robot_status         robot_status;    //機器人整體運行狀態檢查碼  
    struct Robot_lcd_status     lcd_status;      //lcd輸出內容
};
#endif