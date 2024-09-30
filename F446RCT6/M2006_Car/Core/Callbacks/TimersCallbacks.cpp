#include "tim.h"

#include "Motor_Shared.hpp"
#include "PID_Shared.hpp"
#include "RemoteMapping.hpp"
#include "SSD1306_Shared.hpp"
#include "Status.hpp"
#include "strings.hpp"

// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM7) { // 1ms
		// Watchdogs
		if (remoteControl.Tick()) {
			// 如果超时，强制重置 controllerData
			controllerData = { 0, 0, 0, 0, RemoteControl::SwitchPosition::Unknown, RemoteControl::SwitchPosition::Unknown, 0 };
		}

		leftMotorWatchDog.Tick();
		rightMotorWatchDog.Tick();

		return;
	}

	if (htim->Instance == TIM6) { // 100ms
		// 更新当前系统状态和速度模式
		MapRemoteToStatusAndMode();

		// 更新电机速度
		MapRemoteToMotorSpeed(controllerData, leftTarget, rightTarget);

		// PID 计算
		leftMotor.SetCurrent(leftPID.Update(-leftTarget, leftSpeed));
		rightMotor.SetCurrent(rightPID.Update(rightTarget, rightSpeed));

		auto sent_result = Motor::SendControlCommand(&leftMotor, &rightMotor);
		UNUSED(sent_result); // for debug

		// 串口发送数据
		uint8_t txData[16] = { 0 };
		*reinterpret_cast<float *>(txData) = leftTarget;
		*reinterpret_cast<float *>(txData + 4) = rightTarget;
		*reinterpret_cast<float *>(txData + 8) = leftSpeed;
		*reinterpret_cast<float *>(txData + 12) = rightSpeed;
		HAL_UART_Transmit_DMA(&huart1, txData, sizeof(txData));

		// 更新屏幕
		char buffer[8] = { 0 };

		ssd1306.Clear();

		/*
			@todo 展示信息
			RC: n, Motor: OK/Err     // 遥控状态；电机状态                 高 11 -> 0, 0
			Status: Stop/1 Stick/2 Stick/EB/Err  // 系统状态：停止/运行/紧急停止/错误   高 12 -> 0, 11
			Left: ttt.t,sss.s        // 左电机目标，左电机转速             高 10 -> 0, 23
			Right: ttt.t,sss.s      // 右电机目标，右电机转速             高 10 -> 0, 33
			Mode: Off/Low/High/EB    // 速度模式：关闭/低速/高速/紧急停止   高 10 -> 0, 43
			Stick: x, y              // 右摇杆坐标                        高 10 -> 0, 53
		*/

		ssd1306.SetCursor(0, 0);
		ssd1306.WriteString("RC: ", SSD1306Fonts::Font_7x10);
		uint8ToString(static_cast<uint8_t>(remoteControl.GetStatus()), buffer);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
		ssd1306.WriteString(", Motor: ", SSD1306Fonts::Font_7x10);
		MotorStatusToString(buffer);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 11);
		ssd1306.WriteString("Status: ", SSD1306Fonts::Font_7x10);
		SystemStatusToString(status, buffer);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 23);
		ssd1306.WriteString("Left: ", SSD1306Fonts::Font_7x10);
		floatToString(leftTarget, buffer, 1);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
		ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
		floatToString(leftSpeed, buffer, 1);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 33);
		ssd1306.WriteString("Right: ", SSD1306Fonts::Font_7x10);
		floatToString(rightTarget, buffer, 1);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
		ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
		floatToString(rightSpeed, buffer, 1);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 43);
		ssd1306.WriteString("Mode: ", SSD1306Fonts::Font_7x10);
		SpeedModeToString(mode, buffer);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.SetCursor(0, 53);
		ssd1306.WriteString("Stick: ", SSD1306Fonts::Font_7x10);
		int16ToString(status == SystemStatus::RunningAsDualStick ? controllerData.LeftStickY : controllerData.RightStickX, buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
		ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
		int16ToString(controllerData.RightStickY, buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.UpdateScreen();

		return;
	}
}
