#include "PIDInit.h"


void LC_PID_Init(PIDController *pid) {
    // 设置 PID 参数
    pid->Kp = 50.0f;
    pid->Ki = 350.0f;
    pid->Kd = 1.0f;
    pid->tau = 0.1f;

    // 设置输出限制
    pid->limMin = -10000;
    pid->limMax = 10000;

    // 设置积分限制
    pid->limMinInt = -50000;
    pid->limMaxInt = 50000;

    // 设置采样时间
    pid->T = 0.001f;

    // 初始化 PID 控制器
    PIDController_Init(pid);
}