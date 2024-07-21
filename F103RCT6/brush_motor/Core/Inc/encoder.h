#pragma once

#include "tim.h"

// 编码器结构体
typedef struct {
	TIM_HandleTypeDef *HTim; // 定时器句柄指针
	int32_t LastCount;       // 上一次的计数值
	int32_t OverflowCount;   // 溢出计数
	uint32_t LastTime;       // 上次更新时间
	float Speed;             // 计算得到的速度
} Encoder;


/**
 * @brief 初始化编码器
 * @param encoder: 编码器结构体指针
 * @param htim: 编码器定时器句柄指针
 * @return 初始化状态
 */
HAL_StatusTypeDef Encoder_Init(Encoder *encoder, TIM_HandleTypeDef *htim);

/**
 * @brief 更新编码器数据
 * @param encoder: 编码器结构体指针
 * @note 定时调用，如在 HAL_TIM_PeriodElapsedCallback 函数中调用
 */
void Encoder_Update(Encoder *encoder);

/**
 * @brief 编码器溢出回调函数
 * @param htim: 定时器句柄指针
 * @param encoder: 编码器结构体指针
 * @note 在 HAL_TIM_PeriodElapsedCallback 函数中无条件调用
 */
void EncoderOverflowCallback(const TIM_HandleTypeDef *htim, Encoder *encoder);