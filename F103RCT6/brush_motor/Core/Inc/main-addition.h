#pragma once

#include "PID.h"
#include "community.h"
#include "encoder.h"
#include "motor.h"

#include "main.h"

bool MotorSpeed_SentCallback(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);

// 四个电机
extern Motor motor0;
// extern Motor motor1;
// extern Motor motor2;
// extern Motor motor3;

// 四个编码器
extern Encoder encoder0;
// extern Encoder encoder1;
// extern Encoder encoder2;
// extern Encoder encoder3;

// 四个 PID
extern PIDController pid0;
// extern PIDController pid1;
// extern PIDController pid2;
// extern PIDController pid3;

// 电机速度发送
extern Community community;

extern int16_t target_speed;
// extern int16_t round_speed;
extern int8_t speed_step;

extern uint8_t sent_times;

inline bool MotorSpeed_SentCallback(CAN_RxHeaderTypeDef *rxHeader,
                                    uint8_t *rxData) {
  if (rxHeader->StdId == 0x200) {
    // 如果接收到的是 ID 为 0x200 的一字节 0x00 数据
    if (rxHeader->DLC == 1 && rxData[0] == 0x00) {
      // OK
      sent_times--;
    }
    return true;
  }
  return false;
}