#pragma once

#include <cstring>

#include "can.h"
#include "Community.h"
#include "WatchDog.h"

// 电机实际速度
extern float leftFrontSpeed;   // 左前轮速度
extern float rightFrontSpeed;  // 右前轮速度
extern float leftBackSpeed;    // 左后轮速度
extern float rightBackSpeed;   // 右后轮速度

// 电机目标速度
extern float leftFrontTarget;  // 左前轮目标速度
extern float rightFrontTarget; // 右前轮目标速度
extern float leftBackTarget;   // 左后轮目标速度
extern float rightBackTarget;  // 右后轮目标速度

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);

// 电机速度接收
extern Community community;