#pragma once

#include <cstring>

#include "Community.h"
#include "WatchDog.h"
#include "can.h"

// 电机实际速度
extern float leftSpeed;  // 左轮速度
extern float rightSpeed; // 右轮速度

// 电机目标速度
extern float leftTarget;  // 左轮目标速度
extern float rightTarget; // 右轮目标速度

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader,
                                        uint8_t *rxData);

// 电机速度接收
extern Community community;