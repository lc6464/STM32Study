#pragma once

#include "tim.h"

// 电机结构体
typedef struct {
	TIM_HandleTypeDef *HTim; // 定时器句柄指针
	uint32_t ChannelA; // 定时器通道 A
	uint32_t ChannelB; // 定时器通道 B
} Motor;

// 电机操作结果
typedef struct {
	HAL_StatusTypeDef ChannelA; // 通道 A 操作结果
	HAL_StatusTypeDef ChannelB; // 通道 B 操作结果
} Motor_StatusTypeDef;


/**
 * @brief 初始化电机结构体
 * @param motor 电机结构体指针
 * @param htim 定时器句柄指针
 * @param channelA 定时器通道 A
 * @param channelB 定时器通道 B
 */
void Motor_Init(Motor *motor, TIM_HandleTypeDef *htim, const uint32_t channelA, const uint32_t channelB);

/**
 * @brief 启动电机及其控制器（启动 PWM 输出）
 * @param motor 电机结构体
 * @return 启动状态
 */
Motor_StatusTypeDef Motor_Start(const Motor motor);

/**
 * @brief 停止电机及其控制器（停止 PWM 输出）
 * @param motor 电机结构体
 * @return 停止状态
 */
Motor_StatusTypeDef Motor_Stop(const Motor motor);

/**
 * @brief 设置电机速度
 * @param motor 电机结构体
 * @param speed 速度值, 范围为 [-1000, 1000]
 * @note 速度为正时, 电机正向旋转; 速度为负时, 电机反向旋转; 速度为 0 时, 电机刹车
 * @return 设置状态
 */
Motor_StatusTypeDef Motor_SetSpeed(const Motor motor, const int16_t speed);