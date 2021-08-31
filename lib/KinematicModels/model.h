#pragma once


//用于标识里程信息
struct Odom
{
    float x;
    float y;
    float z;
    float vel_x;
    float vel_y;
    float vel_z;
};

// 运动模型接口， 继承该类实现对应的模型
struct Model
{
    // 构造函数
    Model() {}
    Model(float _wheel_radius, float _body_radius): wheel_radius(_wheel_radius), body_radius(_body_radius){}

    // 参数更新接口
    void set(float _wheel_radius, float _body_radius) {
        wheel_radius = _wheel_radius;
        body_radius = _body_radius;
    }

    // 析构函数
    ~Model() {}

    //robot speed ------------> motor speed 运动解算 把给到机器人的速度分解为各个轮子速度
    virtual void motion_solver(float* robot_speed, float* motor_speed) = 0;

    //motor speed-------------> robot speed 反解算， 把各个轮子的速度转为机器人的速度 ，这里通过固定时间间隔转为里程
    //interval  ms
    virtual void get_odom(struct Odom* odom, float* motor_dis, unsigned long interval) = 0;

protected:
    float wheel_radius; // 轮子半径
    float body_radius;  // 机器人半径（泛指）
};
