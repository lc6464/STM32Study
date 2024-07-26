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

		float leftResult = leftPID.Update(leftTarget, leftSpeed);
		float rightResult = rightPID.Update(rightTarget, rightSpeed);

		leftMotor.SetVoltage(static_cast<int16_t>(leftResult));
		rightMotor.SetVoltage(static_cast<int16_t>(rightResult));

		// 发送电机速度反馈
		auto mailbox = Mailboxes::Create();
		auto sent_result = community.SendSpeed(0x004, mailbox, leftSpeed, rightSpeed);
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