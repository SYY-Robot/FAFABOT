# [No.2 麥克納姆輪ROS SLAM小車](https://github.com/KUBOT-Robot/FAFABOT/tree/FAFABOT-No.2)

<img src="https://github.com/KUBOT-Robot/FAFABOT/blob/resource/FAFABOT-No2/2.jpg?raw=true" width="700">

>使用Raspberry Pi 4搭配Xtarkr OpenCRP V2.0控制板(STM32F103RCTX), Rplidar A1進行ROS SLAM, Navigation. 上位機由kubot_ros - galiray2 model 修改.

<img src="https://github.com/KUBOT-Robot/FAFABOT/blob/resource/FAFABOT-No2/13.png?raw=true" width="700">

>建立遠端主從關係, 於遠端電腦觀看RViZ的畫面.
>車端啟動roslaunch kubot_slam_2d gmapping_with_robot.launch.

<img src="https://github.com/KUBOT-Robot/FAFABOT/blob/resource/FAFABOT-No2/14.gif?raw=true" width="700">

>使用gmapping與teb_local_planner進行自動導航.