#include <algorithm>
#include <cmath>

#include "Community_Shared.h"
#include "Mailboxes.h"
#include "RemoteControl_Shared.h"
#include "SSD1306_Shared.h"
#include "Status.h"
#include "strings.h"
#include "tim.h"


// 声明上一次的电机速度为静态变量
static float lastLeftMotorSpeed = 0.0f;
static float lastRightMotorSpeed = 0.0f;

// 映射遥控器输入到电机速度
void MapRemoteToMotorSpeed(int16_t remoteX, int16_t remoteY, float &leftMotorSpeed, float &rightMotorSpeed) {
	constexpr float MAX_REMOTE_VALUE = 660.0f;
	constexpr float MIN_INPUT_THRESHOLD = 20.0f;
	constexpr float TURN_THRESHOLD = 0.90f;
	constexpr float Y_THRESHOLD_FOR_SPIN = 0.2f;
	constexpr float SMOOTH_FACTOR = 0.2f;

	// 紧急停车模式下直接停止
	if (mode == SpeedMode::EmergencyBrake) {
		leftMotorSpeed = 0.0f;
		rightMotorSpeed = 0.0f;
		lastLeftMotorSpeed = 0.0f;
		lastRightMotorSpeed = 0.0f;
		return;
	}

	// Stop 模式下缓慢停止
	if (mode == SpeedMode::Off) {
		if (leftMotorSpeed > 0.0f) {
			leftMotorSpeed -= std::min(20.0f, leftMotorSpeed);
		} else if (leftMotorSpeed < 0.0f) {
			leftMotorSpeed += std::min(20.0f, -leftMotorSpeed);
		}

		if (rightMotorSpeed > 0.0f) {
			rightMotorSpeed -= std::min(20.0f, rightMotorSpeed);
		} else if (rightMotorSpeed < 0.0f) {
			rightMotorSpeed += std::min(20.0f, -rightMotorSpeed);
		}
		return;
	}

	if (remoteX == 0 && remoteY == 0) {
		leftMotorSpeed = 0.0f;
		rightMotorSpeed = 0.0f;
		lastLeftMotorSpeed = 0.0f;
		lastRightMotorSpeed = 0.0f;
		return;
	}

	constexpr float LOW_SPEED_MAX = 300.0f;
	constexpr float HIGH_SPEED_MAX = 500.0f;
	const float maxTargetSpeed = (mode == SpeedMode::Low) ? LOW_SPEED_MAX : HIGH_SPEED_MAX;

	// 限制遥控器输入范围
	remoteX = std::clamp(remoteX, static_cast<int16_t>(-MAX_REMOTE_VALUE), static_cast<int16_t>(MAX_REMOTE_VALUE));
	remoteY = std::clamp(remoteY, static_cast<int16_t>(-MAX_REMOTE_VALUE), static_cast<int16_t>(MAX_REMOTE_VALUE));

	// 计算遥控器输入的模长
	const float magnitude = std::sqrt(remoteX * remoteX + remoteY * remoteY);

	// 如果输入太小，认为是静止状态
	if (magnitude < MIN_INPUT_THRESHOLD) {
		leftMotorSpeed = 0.0f;
		rightMotorSpeed = 0.0f;
		lastLeftMotorSpeed = 0.0f;
		lastRightMotorSpeed = 0.0f;
		return;
	}

	// 将模长映射到电机速度范围
	const float speed = (magnitude / MAX_REMOTE_VALUE) * maxTargetSpeed;

	// 计算方向角（弧度）
	const float angle = std::atan2(remoteY, remoteX);

	// 判断是否需要执行原地掉头或小半径掉头
	if (std::abs(remoteX) > TURN_THRESHOLD * magnitude) {
		if (std::abs(remoteY) < Y_THRESHOLD_FOR_SPIN * magnitude) {
			// 原地掉头模式
			if (remoteX > 0) {
				// 向右原地掉头
				leftMotorSpeed = speed;
				rightMotorSpeed = -speed;
			} else {
				// 向左原地掉头
				leftMotorSpeed = -speed;
				rightMotorSpeed = speed;
			}
		} else {
			// 小半径掉头模式
			const float turnRatio = std::abs(static_cast<float>(remoteY) / remoteX);
			const float innerWheelSpeed = speed * turnRatio;
			const float outerWheelSpeed = speed;

			if (remoteX > 0) {
				// 向右小半径掉头
				leftMotorSpeed = outerWheelSpeed;
				rightMotorSpeed = -innerWheelSpeed;
			} else {
				// 向左小半径掉头
				leftMotorSpeed = -innerWheelSpeed;
				rightMotorSpeed = outerWheelSpeed;
			}

			// 根据 Y 轴方向决定前进还是后退
			if (remoteY < 0) {
				leftMotorSpeed = -leftMotorSpeed;
				rightMotorSpeed = -rightMotorSpeed;
			}
		}
	} else {
		// 正常行驶模式
		const float turnFactor = std::cos(angle);

		// 计算左右轮速度
		leftMotorSpeed = speed * (1.0f + turnFactor);
		rightMotorSpeed = speed * (1.0f - turnFactor);

		// 考虑前进和后退
		if (remoteY < 0) {
			leftMotorSpeed = -leftMotorSpeed;
			rightMotorSpeed = -rightMotorSpeed;
		}
	}

	// 限制电机速度范围，顺便取反
	leftMotorSpeed = -std::clamp(leftMotorSpeed, -maxTargetSpeed, maxTargetSpeed);
	rightMotorSpeed = -std::clamp(rightMotorSpeed, -maxTargetSpeed, maxTargetSpeed);

	// 应用平滑过渡
	leftMotorSpeed = lastLeftMotorSpeed * (1 - SMOOTH_FACTOR) + leftMotorSpeed * SMOOTH_FACTOR;
	rightMotorSpeed = lastRightMotorSpeed * (1 - SMOOTH_FACTOR) + rightMotorSpeed * SMOOTH_FACTOR;

	// 更新上一次的速度
	lastLeftMotorSpeed = leftMotorSpeed;
	lastRightMotorSpeed = rightMotorSpeed;
}

// 映射遥控状态到系统状态和速度模式
void MapRemoteToStatusAndMode() {
	auto rcStatus = remoteControl.GetStatus();
	if (rcStatus != RemoteControl::Status::Running) {
		status = SystemStatus::Error;
		mode = SpeedMode::EmergencyBrake;
		return;
	}

	// 右开关 上下 -> EB + EB  中 -> 由左开关决定
	// 左开关 上中下 -> Stop + Off / Run + Low / Run + High
	if (controllerData.RightSwitch != RemoteControl::SwitchPosition::Middle) {
		status = SystemStatus::EmergencyBrake;
		mode = SpeedMode::EmergencyBrake;
		return;
	}

	if (controllerData.LeftSwitch == RemoteControl::SwitchPosition::Up) {
		status = SystemStatus::Stopped;
		mode = SpeedMode::Off;
	} else {
		status = SystemStatus::Running;
		mode = controllerData.LeftSwitch == RemoteControl::SwitchPosition::Middle ? SpeedMode::Low : SpeedMode::High;
	}
}

// 定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM7) { // 10ms
		// Watchdogs
		remoteControl.Tick();

		return;
	}

	if (htim->Instance == TIM6) { // 100ms
		// Watchdogs
		communityWatchDog.Tick();

		// 更新当前系统状态和速度模式
		MapRemoteToStatusAndMode();

		// 更新电机速度
		MapRemoteToMotorSpeed(controllerData.RightStickX, controllerData.RightStickY, leftTarget, rightTarget);

		// 发送电机速度
		auto mailbox = Mailboxes::Create();
		auto sent_result = community1.SendSpeed(0x002, mailbox, leftTarget, rightTarget);
		UNUSED(sent_result);

		// 更新屏幕
		char buffer[8] = { 0 };

		ssd1306.Clear();

		/*
			@todo 展示信息
			RC: n, Motor: OK/Err     // 遥控状态；电机状态                 高 11 -> 0, 0
			Status: Stop/Run/EB/Err  // 系统状态：停止/运行/紧急停止/错误   高 12 -> 0, 11
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
		int16ToString(controllerData.RightStickX, buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);
		ssd1306.WriteString(",", SSD1306Fonts::Font_7x10);
		int16ToString(controllerData.RightStickY, buffer, 0);
		ssd1306.WriteString(buffer, SSD1306Fonts::Font_7x10);

		ssd1306.UpdateScreen();

		return;
	}
}