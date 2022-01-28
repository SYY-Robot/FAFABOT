#ifndef PIBOT_OMNI3_H_
#define PIBOT_OMNI3_H_

#include "model.h"
#include "math.h"

//定义Motor数目
#define MOTOR_COUNT 3

/*
		  x
          ^
          |
          |
          |
y<--------
          1
        -   -
       -     -
      -       -
     -         -
    -           -
   -             -
  -               -
 -                 -
2-------------------3
*/


#define sqrt_of_3 1.732f

// 3轮全向模型接口实现
class Omni3 : public Model
{
public:
	Omni3() {}
	Omni3(float _wheel_radius, float _body_radius) : Model(_wheel_radius, _body_radius) {}
	
	//运动解算 把给到机器人的速度分解为各个轮子速度
	void motion_solver(float* robot_speed, float* motor_speed) {
		//	robot_speed[0] x	robot_speed[1] y	robot_speed[2] z
		motor_speed[0] = (robot_speed[1] + robot_speed[2] * body_radius)/ wheel_radius;
		motor_speed[1] = (-robot_speed[0]*sqrt_of_3*0.5 - robot_speed[1]*0.5 + robot_speed[2] * body_radius) / wheel_radius;
		motor_speed[2] = (robot_speed[0]*sqrt_of_3*0.5 - robot_speed[1]*0.5 + robot_speed[2] * body_radius) / wheel_radius;
    }
	
	//反解算， 把各个轮子的速度转为机器人的速度 ，这里通过固定时间间隔转为里程
	void get_odom(struct Odom* odom, float* motor_dis, unsigned long interval) {
		if (motor_dis[0]!=0 || motor_dis[1]!=0 || motor_dis[2]!=0){
			//speed
			float dvx = (-motor_dis[1]+motor_dis[2])*sqrt_of_3/3.0f;
			float dvy = (motor_dis[0]*2-motor_dis[1]-motor_dis[2])/3.0f;
			float dvth = (motor_dis[0]+motor_dis[1]+motor_dis[2])/ (3 * body_radius);
			odom->vel_x = dvx / interval; 
			odom->vel_y = dvy / interval; 
			odom->vel_z = dvth / interval; 

			float th = odom->z;

			//odometry
			float dx = (-sin(th)*motor_dis[0]*2+(-sqrt_of_3*cos(th)+sin(th))*motor_dis[1]+(sqrt_of_3*cos(th)+sin(th))*motor_dis[2])/3.0f;
			float dy = (cos(th)*motor_dis[0]*2+(-sqrt_of_3*sin(th)-cos(th))*motor_dis[1]+(sqrt_of_3*sin(th)-cos(th))*motor_dis[2])/3.0f;
			float dth = (motor_dis[0]+motor_dis[1]+motor_dis[2])/ (3 * body_radius);
			
			odom->x += dx;
			odom->y += dy;
			odom->z += dth;
		}
	}
};

#endif
