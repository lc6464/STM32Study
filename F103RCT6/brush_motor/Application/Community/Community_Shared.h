#pragma once

#include <cstring>

#include "can.h"
#include "Community.h"
#include "WatchDog.h"

inline bool MotorSpeed_ReceivedCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);

// 电机速度接收
extern Community community;