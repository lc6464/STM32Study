#include <algorithm>
#include <cmath>
#include <cstring>

#include "tim.h"

#include "Callbacks_Shared.h"
#include "Community_Shared.h"
#include "Encoder_Shared.h"
#include "Mailboxes.h"
#include "Motor_Shared.h"
#include "PID_Shared.h"

// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM7) { // 10ms
    // Watchdogs
    systemWatchDog.Tick();

    return;
  }

  if (htim->Instance == TIM6) { // 100ms
    leftSpeed = leftEncoder.Update();
    rightSpeed = rightEncoder.Update();

    float leftResult = 0;
    float rightResult = 0;

    if (systemStatus == Status::Error) {
      leftPID.Reset();
      rightPID.Reset();
      leftTarget = 0;
      rightTarget = 0;
    } else {
      // PID 控制

      if (std::abs(leftTarget) < 10 && std::abs(leftSpeed) < 30) {
        leftPID.Reset();
        leftTarget = 0;
      } else {
        leftResult = leftPID.Update(leftTarget, leftSpeed);
      }

      if (std::abs(rightTarget) < 10 && std::abs(rightSpeed) < 30) {
        rightPID.Reset();
        rightTarget = 0;
      } else {
        rightResult = rightPID.Update(rightTarget, rightSpeed);
      }
    }

    leftMotor.SetVoltage(static_cast<int16_t>(leftResult));
    rightMotor.SetVoltage(static_cast<int16_t>(rightResult));

    // 发送电机速度反馈
    auto mailbox = Mailboxes::Create();
    auto sent_result =
        community.SendSpeed(0x004, mailbox, leftSpeed, rightSpeed);
    UNUSED(sent_result); // for debug

    if (systemStatus == Status::Error) {
      // 错误，灯灭
      HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    } else {
      // 正常，灯闪烁
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }

    return;
  }

  // 编码器定时器溢出更新
  if (leftEncoder.OverflowCallback(htim)) {
    return;
  }

  if (rightEncoder.OverflowCallback(htim)) {
    return;
  }
}