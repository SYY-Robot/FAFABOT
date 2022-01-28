#ifndef PIBOT_MOTOR_CONTROLLER_H_
#define PIBOT_MOTOR_CONTROLLER_H_

// 电机控制器接口
class MotorController
{
public:
	//初始化
	virtual void init()=0;

	/*控制电机 
	param: pwm 标识电机速度 >0 正转  <0 反转  =0 停止
	*/
	virtual void control(short pwm)=0;
};

#endif