#include "Status.h"

namespace Status {
	SystemStatus systemStatus = SystemStatus::Idle; // 系统状态，默认为空闲状态
	DebugStatus debugStatus = DebugStatus::None;   // 调试状态，默认为无调试状态

	// 电机实际速度
	float leftFrontSpeed = 0;   // 左前轮速度
	float rightFrontSpeed = 0;  // 右前轮速度
	float leftBackSpeed = 0;    // 左后轮速度
	float rightBackSpeed = 0;   // 右后轮速度

	// 电机目标速度
	float leftFrontTarget = 0;  // 左前轮目标速度
	float rightFrontTarget = 0; // 右前轮目标速度
	float leftBackTarget = 0;   // 左后轮目标速度
	float rightBackTarget = 0;  // 右后轮目标速度
};