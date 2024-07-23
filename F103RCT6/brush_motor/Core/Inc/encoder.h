#pragma once

#include "tim.h"

/**
 * @brief 编码器类，适用于不超过16位的编码器模式定时器
 */
class Encoder {
public:
	/**
	 * @brief 构造函数，初始化编码器
	 * @param htim 编码器定时器句柄指针
	 */
	Encoder(TIM_HandleTypeDef *htim);

	/**
	 * @brief 启动定时器编码器
	 * @return 启动状态
	 */
	HAL_StatusTypeDef Start();

	/**
	 * @brief 停止定时器编码器
	 * @return 停止状态
	 */
	HAL_StatusTypeDef Stop();

	/**
	 * @brief 更新编码器数据
	 * @note 定时调用，如在 HAL_TIM_PeriodElapsedCallback 函数中调用
	 */
	void Update();

	/**
	 * @brief 编码器溢出回调函数
	 * @param htim 中断的定时器句柄指针
	 * @note 在 HAL_TIM_PeriodElapsedCallback 函数中无条件调用
	 */
	void OverflowCallback(const TIM_HandleTypeDef *htim);

	/**
	 * @brief 获取当前速度
	 * @return 当前速度值
	 */
	float GetSpeed() const { return _speed; }

private:
	static constexpr float FILTER_ALPHA = 0.3f;  // 低通滤波器系数

	TIM_HandleTypeDef *_htim;     // 定时器句柄指针
	uint16_t _lastCount;          // 上一次的计数值，实际不会超过 65535
	int8_t _overflowCount;        // 溢出计数
	uint32_t _lastTime;           // 上次运行时间
	float _speed;                 // 计算得到的速度
	bool _isStopped;              // 是否停止
	uint32_t _lastUpdateTime;     // 上次更新时间
};