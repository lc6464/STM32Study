#include "Motor.h"

Motor::Motor(TIM_HandleTypeDef *htim, uint32_t channelA, uint32_t channelB)
	: _htim(htim), _channelA(channelA), _channelB(channelB) {
	assert_param(htim != nullptr);
	assert_param(channelA != 0);
	assert_param(channelB != 0);
}

Motor::StatusTypeDef Motor::Start() {
	StatusTypeDef result;
	result.ChannelA = HAL_TIM_PWM_Start(_htim, _channelA);
	result.ChannelB = HAL_TIM_PWM_Start(_htim, _channelB);
	return result;
}

Motor::StatusTypeDef Motor::Stop() {
	StatusTypeDef result;
	result.ChannelA = HAL_TIM_PWM_Stop(_htim, _channelA);
	result.ChannelB = HAL_TIM_PWM_Stop(_htim, _channelB);
	return result;
}

void Motor::SetSpeed(int16_t speed) {
	if (speed > 0 && speed <= 1000) {
		// 正向旋转: CH3 PWM, CH4 高电平
		__HAL_TIM_SET_COMPARE(_htim, _channelA, 1000 - speed);
		__HAL_TIM_SET_COMPARE(_htim, _channelB, 2000);
	} else if (speed < 0 && speed >= -1000) {
		// 反向旋转: CH3 高电平, CH4 PWM
		__HAL_TIM_SET_COMPARE(_htim, _channelA, 2000);
		__HAL_TIM_SET_COMPARE(_htim, _channelB, 1000 + speed);
	} else {
		// 刹车: 两个通道均为高电平
		__HAL_TIM_SET_COMPARE(_htim, _channelA, 2000);
		__HAL_TIM_SET_COMPARE(_htim, _channelB, 2000);
	}
}