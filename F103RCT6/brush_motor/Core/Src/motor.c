#include "motor.h"

/**
 * @brief 初始化电机结构体
 * @param motor 电机结构体指针
 * @param htim 定时器句柄指针
 * @param channelA 定时器通道 A
 * @param channelB 定时器通道 B
 */
void Motor_Init(Motor *motor, TIM_HandleTypeDef *htim, const uint32_t channelA, const uint32_t channelB) {
	assert_param(motor != NULL);
	assert_param(htim != NULL);
	assert_param(channelA != 0);
	assert_param(channelB != 0);

	motor->HTim = htim;
	motor->ChannelA = channelA;
	motor->ChannelB = channelB;
}

/**
 * @brief 启动电机及其控制器（启动 PWM 输出）
 * @param motor 电机结构体
 * @return 启动状态
 */
Motor_StatusTypeDef Motor_Start(const Motor motor) {
	assert_param(motor != NULL);

	Motor_StatusTypeDef result = { 0 };

	result.ChannelA = HAL_TIM_PWM_Start(motor.HTim, motor.ChannelA);
	result.ChannelB = HAL_TIM_PWM_Start(motor.HTim, motor.ChannelB);

	return result;
}

/**
 * @brief 停止电机及其控制器（停止 PWM 输出）
 * @param motor 电机结构体
 * @return 停止状态
 */
Motor_StatusTypeDef Motor_Stop(const Motor motor) {
	assert_param(motor != NULL);

	Motor_StatusTypeDef result = { 0 };

	result.ChannelA = HAL_TIM_PWM_Stop(motor.HTim, motor.ChannelA);
	result.ChannelB = HAL_TIM_PWM_Stop(motor.HTim, motor.ChannelB);

	return result;
}

/**
 * @brief 设置电机速度
 * @param motor 电机结构体
 * @param speed 速度值, 范围为 [-1000, 1000]
 * @note 速度为正时, 电机正向旋转; 速度为负时, 电机反向旋转; 速度为 0 时, 电机刹车
 * @return 设置状态
 */
Motor_StatusTypeDef Motor_SetSpeed(const Motor motor, const int16_t speed) {
	assert_param(motor != NULL);

	Motor_StatusTypeDef result = { 0 };

	if (speed > 0 && speed <= 1000) {
		// 正向旋转: CH3 PWM, CH4 高电平
		result.ChannelA = __HAL_TIM_SET_COMPARE(motor.HTim, motor.ChannelA, 1000 - speed);
		result.ChannelB = __HAL_TIM_SET_COMPARE(motor.HTim, motor.ChannelB, 2000);
	} else if (speed < 0 && speed >= -1000) {
		// 反向旋转: CH3 高电平, CH4 PWM
		result.ChannelA = __HAL_TIM_SET_COMPARE(motor.HTim, motor.ChannelA, 2000);
		result.ChannelB = __HAL_TIM_SET_COMPARE(motor.HTim, motor.ChannelB, 1000 + speed);
	} else {
		// 刹车: 两个通道均为高电平
		result.ChannelA = __HAL_TIM_SET_COMPARE(motor.HTim, motor.ChannelA, 2000);
		result.ChannelB = __HAL_TIM_SET_COMPARE(motor.HTim, motor.ChannelB, 2000);
	}

	return result;
}