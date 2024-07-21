#include "encoder.h"

/**
 * @brief 初始化编码器
 * @param encoder: 编码器结构体指针
 * @param htim: 编码器定时器句柄指针
 * @return 初始化状态
 */
HAL_StatusTypeDef Encoder_Init(Encoder *encoder, TIM_HandleTypeDef *htim) {
	assert_param(encoder != NULL);
	assert_param(htim != NULL);

	// 初始化编码器结构体
	encoder->HTim = htim;
	encoder->LastCount = 0;
	encoder->OverflowCount = 0;
	encoder->LastTime = HAL_GetTick();
	encoder->Speed = 0;

	// 重置编码器定时器
	__HAL_TIM_SET_COUNTER(htim, 0);

	// 启动编码器定时器
	return HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

/**
 * @brief 更新编码器数据
 * @param encoder: 编码器结构体指针
 * @note 定时调用，如在 HAL_TIM_PeriodElapsedCallback 函数中调用
 */
void Encoder_Update(Encoder *encoder) {
	assert_param(encoder != NULL);
	assert_param(encoder->HTim != NULL);

	// 读取当前计数值和时间
	int32_t currentCount = (int32_t)__HAL_TIM_GET_COUNTER(encoder->HTim);
	uint32_t currentTime = HAL_GetTick();
	uint8_t isTimeOverflow = currentTime < encoder->LastTime;

	// 计算计数值位移
	int32_t diff = currentCount - encoder->LastCount;
	diff += (encoder->OverflowCount * 65536);
	encoder->OverflowCount = 0;
	encoder->LastCount = currentCount;

	if (diff == 0) {
		// 检查是否超过 300ms 没有更新
		uint64_t timeSinceLastUpdate = currentTime + (isTimeOverflow * UINT32_MAX) - encoder->LastTime;
		if (timeSinceLastUpdate >= 300) {
			encoder->LastCount = 0;
			encoder->OverflowCount = 0;
			encoder->LastTime = currentTime;
			encoder->Speed = 0;

			// 重置编码器定时器
			__HAL_TIM_SET_COUNTER(encoder->HTim, 0);
		}
		return;
	}

	// 计算速度和方向
	uint64_t timeDiff = currentTime + (isTimeOverflow * UINT32_MAX) - encoder->LastTime;
	encoder->LastTime = currentTime;

	// 计算速度（单位：计数/秒）
	//encoder->Speed = (diff * 1000.0f) / timeDiff;
	encoder->Speed = diff;
}

/**
 * @brief 编码器溢出回调函数
 * @param htim: 定时器句柄指针
 * @param encoder: 编码器结构体指针
 * @note 在 HAL_TIM_PeriodElapsedCallback 函数中无条件调用
 */
void EncoderOverflowCallback(const TIM_HandleTypeDef *htim, Encoder *encoder) {
	assert_param(encoder != NULL);
	assert_param(encoder->HTim != NULL);
	assert_param(htim != NULL);

	// 如果是当前编码器的定时器
	if (encoder->HTim->Instance == htim->Instance) {
		// 检查方向
		if (__HAL_TIM_IS_TIM_COUNTING_DOWN(encoder->HTim)) {
			encoder->OverflowCount--;  // 向下计数时溢出
		} else {
			encoder->OverflowCount++;  // 向上计数时溢出
		}
	}
}