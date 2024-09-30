#include "Motor_Shared.hpp"

// 电机控制板状态
MotorStatus leftMotorStatus = MotorStatus::Error;
MotorStatus rightMotorStatus = MotorStatus::Error;

// 电机实际速度
float leftSpeed = 0;   // 左轮速度
float rightSpeed = 0;  // 右轮速度

// 电机目标速度
float leftTarget = 0;  // 左轮目标速度
float rightTarget = 0; // 右轮目标速度

// 减速比：36
constexpr float REDUCTION_RATIO = 36;

// 电机速度接收
Motor leftMotor(hcan1, 1, [](Motor::Feedback feedback) { leftSpeed = feedback.speed / REDUCTION_RATIO; leftMotorWatchDog.Feed(); leftMotorStatus = MotorStatus::OK; });
Motor rightMotor(hcan1, 2, [](Motor::Feedback feedback) { rightSpeed = feedback.speed / REDUCTION_RATIO; rightMotorWatchDog.Feed(); rightMotorStatus = MotorStatus::OK; });

WatchDog leftMotorWatchDog(50, []() { leftMotorStatus = MotorStatus::Error; });
WatchDog rightMotorWatchDog(50, []() { rightMotorStatus = MotorStatus::Error; });
