#include "PID_Shared.h"

// 两个 PID 控制器
PIDController pid0(1.0f, 5.0f, 0.1f, 0.1f, -10000, 10000, -50000, 50000, 0.001f);
PIDController pid1(1.0f, 5.0f, 0.1f, 0.1f, -10000, 10000, -50000, 50000, 0.001f);