#pragma once

#include "Community.h"
#include "can.h"

// 电机实际速度
extern int16_t leftSpeed;  // 左轮速度
extern int16_t rightSpeed; // 右轮速度

// 电机目标速度
extern int16_t leftTarget;  // 左轮目标速度
extern int16_t rightTarget; // 右轮目标速度

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader,
                                        uint8_t *rxData);

// 电机速度接收
extern Community community0;
extern Community community1;