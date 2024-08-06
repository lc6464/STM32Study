#include "RemoteMapping.h"

#include <algorithm>
#include <cmath>

#include "Status.h"

constexpr float MAX_REMOTE_VALUE = 660.0f; // 最大遥控值
constexpr float MIN_INPUT_THRESHOLD = 20.0f; // 最小输入阈值
constexpr float SMOOTH_FACTOR = 0.75f; // 平滑因子

// 定义低速和高速的最大速度
constexpr float LOW_SPEED_MAX = 300.0f;
constexpr float HIGH_SPEED_MAX = 450.0f;

// 声明上一次的电机速度为静态变量
static float lastLeftMotorSpeed = 0.0f;
static float lastRightMotorSpeed = 0.0f;

/**
 * @brief 映射遥控器右杆 XY 输入到电机速度
 * @param[in] rightX 右杆 X 输入
 * @param[in] rightY 右杆 Y 输入
 * @param[out] leftMotorSpeed 左电机速度
 * @param[out] rightMotorSpeed 右电机速度
 * @return 是否进入缓慢停止状态（不再进行后续计算）
 */
bool MapRightStickToMotorSpeed(int16_t rightX, int16_t rightY, float &leftMotorSpeed, float &rightMotorSpeed) {
	// 定义了一些常量，用于控制电机速度和阈值
	constexpr float TURN_THRESHOLD = 0.90f; // 转向阈值
	constexpr float Y_THRESHOLD_FOR_SPIN = 0.2f; // 旋转的 Y 轴阈值

	// 计算遥控器输入的模长
	const float magnitude = std::sqrt(rightX * rightX + rightY * rightY);

	// 如果输入太小，按 0.9 的滤波系数平滑过渡到 0
	if (magnitude < MIN_INPUT_THRESHOLD) {
		// 应用平滑过渡
		leftMotorSpeed = lastLeftMotorSpeed * 0.1;
		rightMotorSpeed = lastRightMotorSpeed * 0.1;

		// 更新上一次的速度
		lastLeftMotorSpeed = leftMotorSpeed;
		lastRightMotorSpeed = rightMotorSpeed;

		return true;
	}

	const float maxTargetSpeed = (mode == SpeedMode::Low) ? LOW_SPEED_MAX : HIGH_SPEED_MAX; // 根据模式选择最大速度

	// 将模长映射到电机速度范围
	const float speed = (magnitude / MAX_REMOTE_VALUE) * maxTargetSpeed;

	// 判断是否需要执行原地掉头或小半径掉头
	if (std::abs(rightX) > TURN_THRESHOLD * magnitude) {
		if (std::abs(rightY) < Y_THRESHOLD_FOR_SPIN * magnitude) {
			// 原地掉头模式
			if (rightX > 0) {
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
			const float turnRatio = std::abs(static_cast<float>(rightY) / rightX);
			const float innerWheelSpeed = speed * turnRatio;
			const float outerWheelSpeed = speed;

			if (rightX > 0) {
				// 向右小半径掉头
				leftMotorSpeed = outerWheelSpeed;
				rightMotorSpeed = -innerWheelSpeed;
			} else {
				// 向左小半径掉头
				leftMotorSpeed = -innerWheelSpeed;
				rightMotorSpeed = outerWheelSpeed;
			}

			// 根据 Y 轴方向决定前进还是后退
			if (rightY < 0) {
				leftMotorSpeed = -leftMotorSpeed;
				rightMotorSpeed = -rightMotorSpeed;
			}
		}
	} else {
		// 正常行驶模式

		// 计算方向角（弧度）
		const float angle = std::atan2(rightY, rightX);

		const float turnFactor = std::cos(angle);

		// 计算左右轮速度
		leftMotorSpeed = speed * (1.0f + turnFactor);
		rightMotorSpeed = speed * (1.0f - turnFactor);

		// 考虑前进和后退
		if (rightY < 0) {
			leftMotorSpeed = -leftMotorSpeed;
			rightMotorSpeed = -rightMotorSpeed;
		}
	}

	// 限制电机速度范围，顺便取反
	leftMotorSpeed = -std::clamp(leftMotorSpeed, -maxTargetSpeed, maxTargetSpeed);
	rightMotorSpeed = -std::clamp(rightMotorSpeed, -maxTargetSpeed, maxTargetSpeed);

	return false;
}

/**
 * @brief 映射遥控器双杆 Y 输入到电机速度
 * @param[in] leftY 左杆 Y 输入
 * @param[in] rightY 右杆 Y 输入
 * @param[out] leftMotorSpeed 左电机速度
 * @param[out] rightMotorSpeed 右电机速度
 * @return 是否进入缓慢停止状态（不再进行后续计算）
 */
bool MapDualStickToMotorSpeed(int16_t leftY, int16_t rightY, float &leftMotorSpeed, float &rightMotorSpeed) {
	// 左右遥控器输入分别映射到电机速度，互不影响

	const float maxTargetSpeed = (mode == SpeedMode::Low) ? LOW_SPEED_MAX : HIGH_SPEED_MAX; // 根据模式选择最大速度

	// 映射左右遥控器输入到电机速度
	if (std::abs(leftY) < MIN_INPUT_THRESHOLD && std::abs(rightY) < MIN_INPUT_THRESHOLD) {
		leftMotorSpeed = lastLeftMotorSpeed * 0.1;
		rightMotorSpeed = lastRightMotorSpeed * 0.1;

		lastLeftMotorSpeed = leftMotorSpeed;
		lastRightMotorSpeed = rightMotorSpeed;

		return true;
	} else {
		leftMotorSpeed = -std::clamp(static_cast<float>(leftY) / MAX_REMOTE_VALUE * maxTargetSpeed, -maxTargetSpeed, maxTargetSpeed);
		rightMotorSpeed = -std::clamp(static_cast<float>(rightY) / MAX_REMOTE_VALUE * maxTargetSpeed, -maxTargetSpeed, maxTargetSpeed);
	}

	return false;
}

/**
 * @brief 映射遥控器输入到电机速度
 * @param[in] controllerData 遥控器控制器数据
 * @param[out] leftMotorSpeed 左电机速度
 * @param[out] rightMotorSpeed 右电机速度
 */
void MapRemoteToMotorSpeed(RemoteControl::ControllerData controllerData, float &leftMotorSpeed, float &rightMotorSpeed) {
	// 紧急停车模式下直接停止
	if (mode == SpeedMode::EmergencyBrake) {
		leftMotorSpeed = 0.0f; // 左电机速度设为0
		rightMotorSpeed = 0.0f; // 右电机速度设为0
		lastLeftMotorSpeed = 0.0f; // 上一次左电机速度设为0
		lastRightMotorSpeed = 0.0f; // 上一次右电机速度设为0
		return; // 结束函数
	}

	// Stop 模式下缓慢停止 或 遥控器输入为 0，按 0.9 的滤波系数平滑过渡到 0
	if (
		mode == SpeedMode::Off
		|| (status == SystemStatus::RunningAsOneStick && controllerData.RightStickX == 0 && controllerData.RightStickY == 0)
		|| (status == SystemStatus::RunningAsDualStick && controllerData.LeftStickY == 0 && controllerData.RightStickY == 0)
		) {
		// 应用平滑过渡
		leftMotorSpeed = lastLeftMotorSpeed * 0.1;
		rightMotorSpeed = lastRightMotorSpeed * 0.1;

		// 更新上一次的速度
		lastLeftMotorSpeed = leftMotorSpeed;
		lastRightMotorSpeed = rightMotorSpeed;

		return;
	}

	// 限制遥控器输入范围，LeftX 没用到就不管了
	controllerData.LeftStickY = std::clamp(controllerData.LeftStickY, static_cast<int16_t>(-MAX_REMOTE_VALUE), static_cast<int16_t>(MAX_REMOTE_VALUE));
	controllerData.RightStickX = std::clamp(controllerData.RightStickX, static_cast<int16_t>(-MAX_REMOTE_VALUE), static_cast<int16_t>(MAX_REMOTE_VALUE));
	controllerData.RightStickY = std::clamp(controllerData.RightStickY, static_cast<int16_t>(-MAX_REMOTE_VALUE), static_cast<int16_t>(MAX_REMOTE_VALUE));

	// 根据遥控器状态映射到电机速度
	if (status == SystemStatus::RunningAsOneStick) {
		if (MapRightStickToMotorSpeed(controllerData.RightStickX, controllerData.RightStickY, leftMotorSpeed, rightMotorSpeed)) {
			return;
		}
	} else {
		if (MapDualStickToMotorSpeed(controllerData.LeftStickY, controllerData.RightStickY, leftMotorSpeed, rightMotorSpeed)) {
			return;
		}
	}

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

	// 右开关 上 -> EB  中 -> 右杆 XY 控制，速度由左开关决定  下 -> 双杆 Y 控制，速度由左开关决定
	// 左开关 上中下 -> Stop + Off / Run + Low / Run + High
	if (controllerData.RightSwitch == RemoteControl::SwitchPosition::Up) {
		status = SystemStatus::EmergencyBrake;
		mode = SpeedMode::EmergencyBrake;
		return;
	}

	if (controllerData.LeftSwitch == RemoteControl::SwitchPosition::Up) {
		status = SystemStatus::Stopped;
		mode = SpeedMode::Off;
	} else {
		status = controllerData.RightSwitch == RemoteControl::SwitchPosition::Middle ? SystemStatus::RunningAsOneStick : SystemStatus::RunningAsDualStick;
		mode = controllerData.LeftSwitch == RemoteControl::SwitchPosition::Middle ? SpeedMode::Low : SpeedMode::High;
	}
}