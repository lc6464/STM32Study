#pragma once

#include "RemoteControl_Shared.hpp"

/**
 * @brief 映射遥控器输入到电机速度
 * @param[in] controllerData 遥控器控制器数据
 * @param[out] leftMotorSpeed 左电机速度
 * @param[out] rightMotorSpeed 右电机速度
 */
void MapRemoteToMotorSpeed(RemoteControl::ControllerData controllerData, float &leftMotorSpeed, float &rightMotorSpeed);

// 映射遥控状态到系统状态和速度模式
void MapRemoteToStatusAndMode();