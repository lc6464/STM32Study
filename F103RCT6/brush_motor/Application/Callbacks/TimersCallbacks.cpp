#include <algorithm>
#include <cmath>
#include <cstring>

#include "tim.h"

#include "Button_Shared.h"
#include "Callbacks_Shared.h"
#include "Community_Shared.h"
#include "Encoder_Shared.h"
#include "Motor_Shared.h"
#include "PID_Shared.h"
#include "Status.h"


// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM7) { // 10ms
		// Watchdogs
		systemWatchDog.Tick();

		// Buttons
		keyButton.Tick(10);

		return;
	}

	if (htim->Instance == TIM6) { // 100ms
		Status::leftFrontSpeed = leftFrontEncoder.Update();
		Status::rightFrontSpeed = rightFrontEncoder.Update();
		Status::leftBackSpeed = leftBackEncoder.Update();
		Status::rightBackSpeed = rightBackEncoder.Update();

		float leftFrontResult = 0;
		float rightFrontResult = 0;
		float leftBackResult = 0;
		float rightBackResult = 0;

		if (Status::systemStatus == Status::SystemStatus::Debug) {
			// 调试状态
			switch (Status::debugStatus) {
			case Status::DebugStatus::LeftFront:
				// 左前电机调试
				rightFrontPID.Reset();
				leftBackPID.Reset();
				rightBackPID.Reset();
				Status::rightFrontTarget = 0;
				Status::rightBackTarget = 0;
				Status::leftBackTarget = 0;

				Status::leftFrontTarget = 240; // 240 rpm
				leftFrontResult = leftFrontPID.Update(Status::leftFrontTarget, Status::leftFrontSpeed);
				break;
			case Status::DebugStatus::RightFront:
				// 右前电机调试
				leftFrontPID.Reset();
				leftBackPID.Reset();
				rightBackPID.Reset();
				Status::leftFrontTarget = 0;
				Status::leftBackTarget = 0;
				Status::rightBackTarget = 0;

				Status::rightFrontTarget = 240; // 240 rpm
				rightFrontResult = rightFrontPID.Update(Status::rightFrontTarget, Status::rightFrontSpeed);
				break;
			case Status::DebugStatus::LeftBack:
				// 左后电机调试
				rightFrontPID.Reset();
				leftFrontPID.Reset();
				rightBackPID.Reset();
				Status::rightFrontTarget = 0;
				Status::leftFrontTarget = 0;
				Status::rightBackTarget = 0;

				Status::leftBackTarget = 240; // 240 rpm
				leftBackResult = leftBackPID.Update(Status::leftBackTarget, Status::leftBackSpeed);
				break;
			case Status::DebugStatus::RightBack:
				// 右后电机调试
				leftFrontPID.Reset();
				leftBackPID.Reset();
				rightFrontPID.Reset();
				Status::leftFrontTarget = 0;
				Status::leftBackTarget = 0;
				Status::rightFrontTarget = 0;

				Status::rightBackTarget = 240; // 240 rpm
				rightBackResult = rightBackPID.Update(Status::rightBackTarget, Status::rightBackSpeed);
				break;
			case Status::DebugStatus::Full:
				// 全部电机调试
				Status::leftFrontTarget = 240;  // 左前轮目标速度
				Status::rightFrontTarget = 240; // 右前轮目标速度
				Status::leftBackTarget = 240;   // 左后轮目标速度
				Status::rightBackTarget = 240;  // 右后轮目标速度
				leftFrontResult = leftFrontPID.Update(Status::leftFrontTarget, Status::leftFrontSpeed);
				rightFrontResult = rightFrontPID.Update(Status::rightFrontTarget, Status::rightFrontSpeed);
				leftBackResult = leftBackPID.Update(Status::leftBackTarget, Status::leftBackSpeed);
				rightBackResult = rightBackPID.Update(Status::rightBackTarget, Status::rightBackSpeed);
				break;
			default:
				// 准备调试或不调试，电机下电
				leftFrontPID.Reset();
				rightFrontPID.Reset();
				leftBackPID.Reset();
				rightBackPID.Reset();
				Status::leftFrontTarget = 0;
				Status::rightFrontTarget = 0;
				Status::leftBackTarget = 0;
				Status::rightBackTarget = 0;
				break;
			}

		} else if (Status::systemStatus != Status::SystemStatus::Running) {
			// 未连接或掉线，电机下电
			leftFrontPID.Reset();
			rightFrontPID.Reset();
			leftBackPID.Reset();
			rightBackPID.Reset();
			Status::leftFrontTarget = 0;
			Status::rightFrontTarget = 0;
			Status::leftBackTarget = 0;
			Status::rightBackTarget = 0;
		} else {
			// PID 控制

			if (std::abs(Status::leftFrontTarget) < 10 && std::abs(Status::leftFrontSpeed) < 30) {
				leftFrontPID.Reset();
				Status::leftFrontTarget = 0;
			} else {
				leftFrontResult = leftFrontPID.Update(Status::leftFrontTarget, Status::leftFrontSpeed);
			}

			if (std::abs(Status::rightFrontTarget) < 10 && std::abs(Status::rightFrontSpeed) < 30) {
				rightFrontPID.Reset();
				Status::rightFrontTarget = 0;
			} else {
				rightFrontResult = rightFrontPID.Update(Status::rightFrontTarget, Status::rightFrontSpeed);
			}

			if (std::abs(Status::leftBackTarget) < 10 && std::abs(Status::leftBackSpeed) < 30) {
				leftBackPID.Reset();
				Status::leftBackTarget = 0;
			} else {
				leftBackResult = leftBackPID.Update(Status::leftBackTarget, Status::leftBackSpeed);
			}

			if (std::abs(Status::rightBackTarget) < 10 && std::abs(Status::rightBackSpeed) < 30) {
				rightBackPID.Reset();
				Status::rightBackTarget = 0;
			} else {
				rightBackResult = rightBackPID.Update(Status::rightBackTarget, Status::rightBackSpeed);
			}
		}

		leftFrontMotor.SetVoltage(static_cast<int16_t>(leftFrontResult));
		rightFrontMotor.SetVoltage(static_cast<int16_t>(rightFrontResult));
		leftBackMotor.SetVoltage(static_cast<int16_t>(leftBackResult));
		rightBackMotor.SetVoltage(static_cast<int16_t>(rightBackResult));

		// 发送电机速度反馈
		uint32_t mailbox = 0;
		community.SendSpeed(0x004, mailbox, Status::leftFrontSpeed, Status::rightFrontSpeed);

		if (Status::systemStatus == Status::SystemStatus::Debug) {
			// 调试，灯亮
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		} else if (Status::systemStatus == Status::SystemStatus::Running) {
			// 正常，灯闪烁
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		} else {
			// 未连接或错误，灯灭
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
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