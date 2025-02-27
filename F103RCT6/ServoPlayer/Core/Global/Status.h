#pragma once

#include <cstdint>

namespace Status {
	// 旋转编码器累加量
	extern int16_t ec11RotationAccumulator;

	// 目标角度
	extern int16_t targetAngle;

	// 舵机角度
	extern int16_t servoAngle;

	// BMI088 角度
	extern float bmi088_angle;
}