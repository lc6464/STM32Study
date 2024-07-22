#include "PIDInit.h"


void LC_PID_Config(PIDController *pid) {
    // 设置 PID 参数
    pid->Kp = 1.0f;
    pid->Ki = 2.0f;
    pid->Kd = 0.1f;
    pid->tau = 0.1f;

    // 设置输出限制
    pid->limMin = -1000;
    pid->limMax = 1000;

    // 设置积分限制
    pid->limMinInt = -800;
    pid->limMaxInt = 800;

    // 设置采样时间
    pid->T = 0.1f;

    // 初始化 PID 控制器
    PIDController_Init(pid);
}