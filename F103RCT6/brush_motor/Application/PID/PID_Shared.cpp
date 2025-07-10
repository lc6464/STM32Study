#include "PID_Shared.h"

// 四个 PID 控制器
PIDController leftFrontPID(2.0f, 0.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);
PIDController rightFrontPID(2.0f, 0.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);
PIDController leftBackPID(2.0f, 0.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);
PIDController rightBackPID(2.0f, 0.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);