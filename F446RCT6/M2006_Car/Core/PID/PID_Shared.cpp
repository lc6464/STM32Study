#include "PID_Shared.hpp"

// 两个 PID 控制器
PIDController leftPID(10.0f, 5.0f, 0.2f, 0.2f, -5000, 5000, -5000, 5000, 0.1f);
PIDController rightPID(10.0f, 5.0f, 0.2f, 0.2f, -5000, 5000, -5000, 5000, 0.1f);