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

	__HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);  // 启用溢出中断

	// 启动编码器定时器
	return HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

/**
 * @brief 更新编码器数据
 * @param encoder: 编码器结构体指针
 * @note 定时调用，如在 HAL_TIM_PeriodElapsedCallback 函数中调用
 */
void Encoder_Update(Encoder *encoder) {
	// 断言确保传入的 encoder 指针和 HTim 指针不为空
	assert_param(encoder != NULL);
	assert_param(encoder->HTim != NULL);

	// 读取当前计数值和时间
	uint16_t currentCount = __HAL_TIM_GET_COUNTER(encoder->HTim); // 当前计数值，实际不会超过 65535
	uint32_t currentTime = HAL_GetTick(); // 获取当前时间（毫秒计时）
	uint8_t isTimeOverflow = currentTime < encoder->LastTime; // 判断是否时间溢出

	// 计算计数值位移
	int32_t diff = (int32_t)currentCount - (int32_t)encoder->LastCount; // 计算当前计数与上次计数之差

	// 处理溢出情况
	diff += (int32_t)encoder->OverflowCount * (int32_t)(encoder->HTim->Instance->ARR + 1); // 增加溢出对应的计数值
	encoder->OverflowCount = 0; // 清零溢出计数

	// 计算时间差（毫秒）
	// uint32_t timeDiff = currentTime - encoder->LastTime; // 被注释掉了，因为没有在下面的代码中用到

	// 更新最后一次的计数和时间
	encoder->LastCount = currentCount; // 更新最后的计数值
	encoder->LastTime = currentTime; // 更新最后的时间值

	if (diff == 0 && !encoder->IsStopped) {
		// 检查是否超过 200ms 没有更新
		uint64_t timeSinceLastUpdate = (uint64_t)currentTime + ((uint64_t)isTimeOverflow * UINT32_MAX) - (uint64_t)encoder->LastUpdateTime; // 计算时间差距
		if (timeSinceLastUpdate >= 200) {
			encoder->LastCount = 0; // 重置计数值
			encoder->OverflowCount = 0; // 重置溢出计数
			encoder->Speed = 0; // 重置速度
			encoder->IsStopped = 1; // 标记编码器停止

			// 重置编码器定时器
			__HAL_TIM_SET_COUNTER(encoder->HTim, 0); // 设置定时器计数值为0
		}
		return;
	}

	// 处理停止状态
	encoder->IsStopped = 0; // 标记编码器没有停止
	encoder->LastUpdateTime = currentTime; // 更新最后更新时间

	/*
	if (timeDiff == 0) {
		return;  // 避免除以零
	}
	*/

	// 计算速度（RPM）
	// float rawSpeed = (float)diff * 60000.0f / (float)(ENCODER_RESOLUTION * GEAR_RATIO * timeDiff); // 原来用来计算速度的代码被注释掉
	float rawSpeed = (float)diff; // 当前实现中直接使用 diff 作为原始速度

	// 应用低通滤波
	encoder->Speed = (float)encoder->Speed * (1.0f - FILTER_ALPHA) + (float)rawSpeed * FILTER_ALPHA; // 低通滤波公式，计算出平滑速度
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