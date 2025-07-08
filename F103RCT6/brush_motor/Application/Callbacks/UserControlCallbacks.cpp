#include "Button_Shared.h"
#include "gpio.h"
#include "Status.h"

// HAL 库外部中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	// 按键中断回调
	keyButton.HandleInterrupt(GPIO_Pin);
}

// void keyButtonPressCallback() {
// 	// 按键按下回调
// }

// void keyButtonReleaseCallback() {
// 	// 按键释放回调
// }

// void keyButtonShortPressCallback() {
// 	// 按键短按回调
// }

void keyButtonLongPressCallback() {
	// 按键长按回调

	// 在 Idle 状态下长按可以进入调试准备
	if (Status::systemStatus == Status::SystemStatus::Idle) {
		Status::systemStatus = Status::SystemStatus::Debug;
		Status::debugStatus = Status::DebugStatus::Preparing;
	} else if (Status::systemStatus == Status::SystemStatus::Debug) {
		// 在调试状态下长按可以进入下一个调试状态
		switch (Status::debugStatus) {
		case Status::DebugStatus::Preparing:
			Status::debugStatus = Status::DebugStatus::LeftFront;
			break;
		case Status::DebugStatus::LeftFront:
			Status::debugStatus = Status::DebugStatus::RightFront;
			break;
		case Status::DebugStatus::RightFront:
			Status::debugStatus = Status::DebugStatus::LeftBack;
			break;
		case Status::DebugStatus::LeftBack:
			Status::debugStatus = Status::DebugStatus::RightBack;
			break;
		case Status::DebugStatus::RightBack:
			Status::debugStatus = Status::DebugStatus::Finished;
			break;
		case Status::DebugStatus::Finished:
			Status::debugStatus = Status::DebugStatus::Full;
			break;
		case Status::DebugStatus::Full:
			Status::debugStatus = Status::DebugStatus::Preparing;
			break;
		default:
			break;
		}
	}
}

// 注册按键回调函数
void RegisterKeyButtonCallbacks() {
	// keyButton.RegisterPressCallback(keyButtonPressCallback);
	// keyButton.RegisterReleaseCallback(keyButtonReleaseCallback);
	// keyButton.RegisterShortPressCallback(keyButtonShortPressCallback);
	keyButton.RegisterLongPressCallback(keyButtonLongPressCallback);
}