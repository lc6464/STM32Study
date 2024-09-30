#pragma once

#include <cstring>

#include "can.h"
#include "Motor.hpp"
#include "WatchDog.hpp"

// 电机控制板状态
enum class MotorStatus {
	OK,
	Error
};

// 电机控制板状态
extern MotorStatus leftMotorStatus;
extern MotorStatus rightMotorStatus;

// 电机控制板状态转字符串
inline void MotorStatusToString(char *buffer) {
	if (leftMotorStatus == MotorStatus::OK && rightMotorStatus == MotorStatus::OK) {
		strcpy(buffer, "OK");
	} else {
		strcpy(buffer, "Err");
	}
}

// 电机输出轴实时速度
extern float leftSpeed;   // 左轮速度
extern float rightSpeed;  // 右轮速度

// 电机目标速度
extern float leftTarget;  // 左轮目标速度
extern float rightTarget; // 右轮目标速度

// 电机速度接收
extern Motor leftMotor;
extern Motor rightMotor;

extern WatchDog leftMotorWatchDog;
extern WatchDog rightMotorWatchDog;
