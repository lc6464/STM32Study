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
		leftFrontSpeed = leftFrontEncoder.Update();
		rightFrontSpeed = rightFrontEncoder.Update();
		leftBackSpeed = leftBackEncoder.Update();
		rightBackSpeed = rightBackEncoder.Update();

		float leftFrontResult = 0;
		float rightFrontResult = 0;
		float leftBackResult = 0;
		float rightBackResult = 0;

		if (systemStatus == Status::Error) {
			leftFrontPID.Reset();
			rightFrontPID.Reset();
			leftBackPID.Reset();
			rightBackPID.Reset();
			leftFrontTarget = 0;
			rightFrontTarget = 0;
			leftBackTarget = 0;
			rightBackTarget = 0;
		} else {
			// PID 控制

			if (std::abs(leftFrontTarget) < 10 && std::abs(leftFrontSpeed) < 30) {
				leftFrontPID.Reset();
				leftFrontTarget = 0;
			} else {
				leftFrontResult = leftFrontPID.Update(leftFrontTarget, leftFrontSpeed);
			}

			if (std::abs(rightFrontTarget) < 10 && std::abs(rightFrontSpeed) < 30) {
				rightFrontPID.Reset();
				rightFrontTarget = 0;
			} else {
				rightFrontResult = rightFrontPID.Update(rightFrontTarget, rightFrontSpeed);
			}

			if (std::abs(leftBackTarget) < 10 && std::abs(leftBackSpeed) < 30) {
				leftBackPID.Reset();
				leftBackTarget = 0;
			} else {
				leftBackResult = leftBackPID.Update(leftBackTarget, leftBackSpeed);
			}

			if (std::abs(rightBackTarget) < 10 && std::abs(rightBackSpeed) < 30) {
				rightBackPID.Reset();
				rightBackTarget = 0;
			} else {
				rightBackResult = rightBackPID.Update(rightBackTarget, rightBackSpeed);
			}
		}

		leftFrontMotor.SetVoltage(static_cast<int16_t>(leftFrontResult));
		rightFrontMotor.SetVoltage(static_cast<int16_t>(rightFrontResult));
		leftBackMotor.SetVoltage(static_cast<int16_t>(leftBackResult));
		rightBackMotor.SetVoltage(static_cast<int16_t>(rightBackResult));

		// 发送电机速度反馈
		// auto mailbox = Mailboxes::Create();
		uint32_t mailbox = 0; // 关掉自动重传后试试
		auto sent_result = community.SendSpeed(0x004, &mailbox, leftFrontSpeed, rightFrontSpeed);

		if (sent_result != HAL_OK) {
			// 发送失败，灯亮
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		} else if (systemStatus == Status::Error) {
			// 错误，灯灭
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		} else {
			// 正常，灯闪烁
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}

		return;
	}

	// 编码器定时器溢出更新
	if (leftFrontEncoder.OverflowCallback(htim)) {
		return;
	}

	if (rightFrontEncoder.OverflowCallback(htim)) {
		return;
	}

	if (leftBackEncoder.OverflowCallback(htim)) {
		return;
	}

	if (rightBackEncoder.OverflowCallback(htim)) {
		return;
	}
}