#include "PID_Shared.h"

// 两个 PID 控制器
PIDController leftPID(2.0f, 0.5f, 0.2f, 0.2f, -1000, 1000, -2000, 2000, 0.1f);
PIDController rightPID(2.0f, 0.5f, 0.2f, 0.2f, -1000, 1000, -2000, 2000, 0.1f);