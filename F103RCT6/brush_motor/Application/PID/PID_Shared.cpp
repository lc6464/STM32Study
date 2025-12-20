#include "PID_Shared.h"

// 四个 PID 控制器
PIDController leftFrontPID(5.0f, 1.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);
PIDController rightFrontPID(5.0f, 1.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);
PIDController leftBackPID(5.0f, 1.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);
PIDController rightBackPID(5.0f, 1.5f, 0.1f, 0.05f, -1000, 1000, -1500, 1500, 0.1f);