#include "EC11.h"

HAL_StatusTypeDef EC11::Start() {
	__HAL_TIM_SET_COUNTER(&_htim, INITIAL_COUNT); // 将计数器初始值设置为初始值 (32768)
	return HAL_TIM_Encoder_Start(&_htim, TIM_CHANNEL_ALL);
}

HAL_StatusTypeDef EC11::Stop() {
	return HAL_TIM_Encoder_Stop(&_htim, TIM_CHANNEL_ALL);
}

int16_t EC11::UpdateRotation() {
	// 读取当前计数值
	uint16_t currentCount = static_cast<uint16_t>(__HAL_TIM_GET_COUNTER(&_htim));

	// 检测旋转方向并调用相应回调
	if (currentCount != INITIAL_COUNT) {
		if (currentCount > INITIAL_COUNT) {
			if (_cwCallback) _cwCallback(currentCount - INITIAL_COUNT);
		} else {
			if (_ccwCallback) _ccwCallback(INITIAL_COUNT - currentCount);
		}
		// 重置计数器为初始值
		__HAL_TIM_SET_COUNTER(&_htim, INITIAL_COUNT);
	}

	return currentCount - INITIAL_COUNT;
}