#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

typedef struct {

  // 控制器增益参数
  float Kp; // 比例增益
  float Ki; // 积分增益
  float Kd; // 微分增益

  // 微分低通滤波器的时间常数
  float tau; // 时间常数，用于影响微分项的响应

  // 输出限制，用于限制控制器的输出值
  float limMin; // 输出最小值限制
  float limMax; // 输出最大值限制

  // 积分器限制，用于防止积分饱和
  float limMinInt; // 积分器最小值限制
  float limMaxInt; // 积分器最大值限制

  // 采样时间（以秒为单位）
  float T; // 采样周期

  // 控制器“记忆”变量，用于存储上一次的误差和测量值
  float integrator;      // 积分器，用于累加误差
  float prevError;       // 上一次的误差，用于计算积分项
  float differentiator;  // 微分器，用于计算微分项
  float prevMeasurement; // 上一次的测量值，用于计算微分项

  // 控制器输出
  float out; // 控制器的输出值

} PIDController;

// PID控制器初始化函数声明
void PIDController_Init(PIDController *pid);

// PID控制器更新函数声明，用于计算新的控制输出
float PIDController_Update(PIDController *pid, float setpoint,
                           float measurement);

#endif