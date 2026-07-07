std_msgs/Header header
float64 leftWheelSpeed  #底盘输出的左轮速度
float64 rightWheelSpeed  #底盘输出的右轮速度
float64 linearSpeed    #底盘计算的线速度值
float64 moveDirect     #dangqian yidong fangxiang
float64 encoderAngleSpeed    #底盘计算所得角速度
float64 imuXAngleSpeed    #imu获取的x方向角速度
float64 imuYAngleSpeed    #imu获取的y方向角速度
float64 imuZAngleSpeed    #imu获取的z方向角速度
float64 imuAngle    #imu获取的方位角
float64 xLinearAcceleration    #imu获取的x方向线加速度
float64 yLinearAcceleration    #imu获取的y方向线加速度
float64 zLinearAcceleration    #imu获取的z方向线加速度
float64 encoderX    #底盘计算角速度：机器人位置-x值
float64 encoderY    #底盘计算角速度：机器人位置-y值
float64 encoderTheta    #底盘计算角速度：机器人方向-theta值
float64 imuX    #imu角速度积分计算：机器人位置-x值
float64 imuY    #imu角速度积分计算：机器人位置-y值
float64 imuTheta    #imu角速度积分计算：机器人方向-theta值
float64 imuX2    #直接用imu方位角计算：机器人位置-x值
float64 imuY2    #直接用imu方位角计算：机器人位置-y值
float64 imuTheta2    #直接用imu方位角计算：机器人方向-theta值
float64 targetPosX   #当前导航的目标点
float64 targetPosY   #当前导航的目标点Y


