#include "PID_Shared.h"

// 两个 PID 控制器
PIDController leftPID(1.5f, 6.0f, 0.3f, 0.2f, -1000, 1000, -2000, 2000, 0.1f);
PIDController rightPID(1.5f, 6.0f, 0.3f, 0.2f, -1000, 1000, -2000, 2000, 0.1f);