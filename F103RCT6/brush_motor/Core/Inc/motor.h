#pragma once

#include "tim.h"

// 电机类
class Motor {
public:
	// 电机操作结果结构体
	struct StatusTypeDef {
		HAL_StatusTypeDef ChannelA; // 通道 A 操作结果
		HAL_StatusTypeDef ChannelB; // 通道 B 操作结果
	};

	/**
	 * @brief 构造函数，初始化电机
	 * @param htim 定时器句柄指针
	 * @param channelA 定时器通道 A
	 * @param channelB 定时器通道 B
	 */
	Motor(TIM_HandleTypeDef *htim, uint32_t channelA, uint32_t channelB);

	/**
	 * @brief 启动电机及其控制器（启动 PWM 输出）
	 * @return 启动状态
	 */
	StatusTypeDef Start();

	/**
	 * @brief 停止电机及其控制器（停止 PWM 输出）
	 * @return 停止状态
	 */
	StatusTypeDef Stop();

	/**
	 * @brief 设置电机速度
	 * @param speed 速度值, 范围为 [-1000, 1000]
	 * @note 速度为正时, 电机正向旋转; 速度为负时, 电机反向旋转; 速度为 0 时, 电机刹车
	 */
	void SetSpeed(int16_t speed);

private:
	TIM_HandleTypeDef *_htim; // 定时器句柄指针
	uint32_t _channelA; // 定时器通道 A
	uint32_t _channelB; // 定时器通道 B
};