#ifndef PIBOT_PID_H_
#define PIBOT_PID_H_

//pid接口
class PID
{
public:
    /* pid构造函数
    * @param input pid输入的地址
    * @param output pid输出的地址
    * @param kp， ki， kd pid参数
    * @param max_output 输出结构的限制值
    */
    PID(float* input, float* feedback, float kp, float ki, float kd, unsigned short max_output);

    // pid计算
    short compute(float interval);

    // 重置pid的值
    void clear();
    
    //更新pid参数
    void update(float kp, float ki, float kd, unsigned short max_output);
private:
    float kp;
    float ki;
    float kd;
    float max_output;
    float* input;
    float* feedback;

    float error; //比例值
    float integra;//积分值
    float derivative;//微分值

    float previous_error; //上一次的输入反馈差值
};

#endif